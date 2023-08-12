#include <GL/glew.h>
#include <unordered_map>
#include <cassert>
#include <fstream>
#include <string>

#include "renderer.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat4.h"

using namespace Renderer;

// OPENGL RENDERER - the only file here w/ a dependency on ogl

void GLAPIENTRY messageCallback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*,const void*);

s32 TextureData::getPixels() const
{
    return width * height;
}

s32 TextureData::getDataLength() const
{
    return getPixels() * stride;
}

std::unordered_map<std::string, ImportSettings::Wrapping> s_wrappingTable
{
    {
        {"Repeat", ImportSettings::Wrapping::Repeat},
        {"MirroredRepeat", ImportSettings::Wrapping::MirroredRepeat},
        {"ClampEdge", ImportSettings::Wrapping::ClampEdge},
        {"ClampBorder", ImportSettings::Wrapping::ClampBorder},
    }
};
std::unordered_map<std::string, ImportSettings::Filtering> s_filterTable
{
    {
        {"Point", ImportSettings::Filtering::Point},
        {"Bilinear", ImportSettings::Filtering::Bilinear},
    }
};

ImportSettings ImportSettings::fromFile(const char* fileName)
{
    std::ifstream stream{};
    stream.open(fileName);

    if (!stream.is_open())
        printf("failed to open texture settings %s", fileName);

    ImportSettings settings;

    std::string wrappingX;
    std::getline(stream, wrappingX);

    std::string wrappingY;
    std::getline(stream, wrappingY);

    std::string textureFiltering;
    std::getline(stream, textureFiltering);

    std::string mipmapFiltering;
    std::getline(stream, mipmapFiltering);

    settings.wrappingX = s_wrappingTable[wrappingX];
    settings.wrappingY = s_wrappingTable[wrappingY];
    settings.textureFiltering = s_filterTable[textureFiltering];
    settings.mipmapFiltering = s_filterTable[mipmapFiltering];

    return settings;
}

void Renderer::init(s32 width, s32 height)
{
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();

    if (err == GLEW_OK)
    {
        printf("Using GLEW as %s\n", glewGetString(GLEW_VERSION));
        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(messageCallback, nullptr);
    }
    else // Something went wrong starting GLEW.
    {
        printf("Error: %s\n", glewGetErrorString(err));
    }
}

void Renderer::resize(s32 width, s32 height)
{
    glViewport(0, 0, width, height);
}

void Renderer::clearScreen(float red, float green, float blue)
{
    glClearColor(red, green, blue, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::clearScreen(Vec3 color)
{
    clearScreen(color.r, color.g, color.b);
}

// A simple callback for handling OpenGL errors.
void GLAPIENTRY messageCallback(GLenum, GLenum type, GLuint, GLenum severity, GLsizei, const GLchar* message, const void*)
{
    printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "",
        type, severity, message);
}

// Textures

GLint getWrapping(ImportSettings::Wrapping wrapping)
{
    GLint result {};

    switch (wrapping)
    {
    case ImportSettings::ClampBorder:
        result = GL_CLAMP_TO_BORDER;
        break;
    case ImportSettings::ClampEdge:
        result = GL_CLAMP_TO_EDGE;
        break;
    case ImportSettings::MirroredRepeat:
        result = GL_MIRRORED_REPEAT;
        break;
    case ImportSettings::Repeat:
        result = GL_REPEAT;
        break;
    }

    return result;
}

TextureHandle Renderer::uploadTexture(const TextureData& data, const ImportSettings& settings)
{
    TextureHandle result {};
    glGenTextures(1, &result);
    GLenum format {};
    GLenum internalFormat {};

    // Flip the image data, since ogl most of the time wants images to read from the bottom left
    // todo: cleanup logic here
    u8* flippedData = new u8[data.getDataLength()];

    for (s32 row = 0; row < data.height; row++)
    {
        s32 unflippedRow = data.height - 1 - row;

        for (s32 column = 0; column < data.width * data.stride; column++)
            flippedData[row * data.width * data.stride + column] = data.data[unflippedRow * data.width * data.stride + column];
    }

    switch (data.format)
    {
    case TextureData::R:
        internalFormat = format = GL_RED;
        break;
    case TextureData::Rgb:
        format = GL_RGB;
        internalFormat = GL_RGB8;
        break;
    case TextureData::Rgba:
        internalFormat = format = GL_RGBA;
        break;
    case TextureData::Rgba8:
        format = GL_RGBA8;
        internalFormat = GL_RGBA;
        break;
    }

    glBindTexture(GL_TEXTURE_2D, result);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat), data.width, data.height, 0, format, GL_UNSIGNED_BYTE, flippedData);
    glGenerateMipmap(GL_TEXTURE_2D);

    GLint wrappingX = getWrapping(settings.wrappingX);
    GLint wrappingY = getWrapping(settings.wrappingY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingX);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingY);

    GLint filterMin {};
    GLint filterMag {};

    { // Determining what the valid mipmap and texture filtering should be.

        auto tf = settings.textureFiltering;
        auto mf = settings.mipmapFiltering;

        if (tf == ImportSettings::Point && mf == ImportSettings::Point)
        {
            filterMin = GL_NEAREST_MIPMAP_NEAREST;
            filterMag = GL_NEAREST;
        }
        else if (tf == ImportSettings::Point && mf == ImportSettings::Bilinear)
        {
            filterMin = GL_NEAREST_MIPMAP_LINEAR;
            filterMag = GL_NEAREST;
        }
        else if (tf == ImportSettings::Bilinear && mf == ImportSettings::Point)
        {
            filterMin = GL_LINEAR_MIPMAP_NEAREST;
            filterMag = GL_LINEAR;
        }
        else if (tf == ImportSettings::Bilinear && mf == ImportSettings::Bilinear)
        {
            filterMin = GL_LINEAR_MIPMAP_LINEAR;
            filterMag = GL_LINEAR;
        }
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);
    return result;
}

void Renderer::deleteTexture(TextureHandle handle)
{
    glDeleteTextures(1, &handle);
}

// Meshes

struct GlMeshData
{
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    s32 indexCount;
};

std::unordered_map<MeshHandle, GlMeshData> g_meshLookup {};
MeshHandle g_nextHandle {};

MeshHandle Renderer::uploadMesh(VertexList vertices, VertexFormat format, IndexList indices)
{
    unsigned int vao, vbo, ebo;

    // Generate gl data.
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // Capture upcoming state in this instance's VAO so we can easily reapply it.
    glBindVertexArray(vao);

    // Fill our buffers with data.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.getSize(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.getSize(), &indices[0], GL_STATIC_DRAW);

    // Define the attributes that correspond with the normal data.
    f32* offset = nullptr;

    for (s32 i = 0; i < format.count; i++)
    {
        glVertexAttribPointer(i, format.data[i], GL_FLOAT, GL_FALSE, static_cast<s32>(sizeof(f32)) * format.sum(), offset);
        glEnableVertexAttribArray(i);
        offset += format.data[i];
    }

    // We are done setting up state, so we can unbind our VAO to clean up.
    glBindVertexArray(0);

    // Save results.
    MeshHandle result { g_nextHandle++ };
    GlMeshData data { vao, vbo, ebo, indices.count};
    g_meshLookup.insert({result, data});
    return result;
}

void Renderer::deleteMesh(MeshHandle handle)
{
    g_meshLookup.erase(handle);
}

// Ensure this lines up with the order of modes in renderer.h
GLenum s_modeTable[] = {
    GL_TRIANGLES, GL_TRIANGLE_STRIP,
};

void Renderer::drawMesh(MeshHandle handle, DrawMode mode)
{
    GlMeshData data { g_meshLookup[handle] };
    glBindVertexArray(data.vao);
    glDrawElements(s_modeTable[mode], data.indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

// Shaders

void Renderer::clearShaders()
{
    useShader(0);
}

unsigned int createShader(const char *source, GLenum type)
{
    // Feed the text data to OpenGL to compile.
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog.data());

        printf("Failure to compile shader: %s\n", errorLog.data());
    }

    return shader;
}

ShaderHandle Renderer::uploadShader(const char* vertexSource, const char* fragmentSource)
{
    unsigned int vertexShader { createShader(vertexSource, GL_VERTEX_SHADER) };
    unsigned int fragmentShader { createShader(fragmentSource, GL_FRAGMENT_SHADER) };

    // Link the loaded vertex and fragment shaders into a usable program.
    ShaderHandle result = glCreateProgram();
    glAttachShader(result, vertexShader);
    glAttachShader(result, fragmentShader);
    glLinkProgram(result);

    // Free the shaders from memory - we only need the program now after linking.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return result;
}

void Renderer::deleteShader(ShaderHandle handle)
{
    glDeleteProgram(handle);
}

unsigned int s_currentHandle{ 0 };

void Renderer::useShader(ShaderHandle handle)
{
    glUseProgram(handle);
    s_currentHandle = handle;
}

void Renderer::setShaderFloat(ShaderHandle handle, const char *name, float value)
{
    assert(s_currentHandle == handle && "Shader must be active before changing values.");
    glUniform1f(glGetUniformLocation(handle, name), value);
}

void Renderer::setShaderDouble(ShaderHandle handle, const char* name, f64 value)
{
    assert(s_currentHandle == handle && "Shader must be active before changing values.");
    glUniform1d(glGetUniformLocation(handle, name), value);
}

void Renderer::setShaderInt(ShaderHandle handle, const char *name, int value)
{
    assert(s_currentHandle == handle && "Shader must be active before changing values.");
    glUniform1i(glGetUniformLocation(handle, name), value);
}

void Renderer::setShaderBool(ShaderHandle handle, const char *name, bool value)
{
    assert(s_currentHandle == handle && "Shader must be active before changing values.");
    glUniform1i(glGetUniformLocation(handle, name), static_cast<int>(value));
}

void Renderer::setShaderVec2(ShaderHandle handle, const char* name, const Vec2& value)
{
    assert(s_currentHandle == handle && "Shader must be active before changing values.");
    glUniform2f(glGetUniformLocation(handle, name), value.x, value.y);
}

void Renderer::setShaderVec3(ShaderHandle handle, const char *name, const Vec3& value)
{
    assert(s_currentHandle == handle && "Shader must be active before changing values.");
    glUniform3f(glGetUniformLocation(handle, name), value.x, value.y, value.z);
}

void Renderer::setShaderVec4(ShaderHandle handle, const char* name, const Vec4& value)
{
    assert(s_currentHandle == handle && "Shader must be active before changing values.");
    glUniform4f(glGetUniformLocation(handle, name), value.x, value.y, value.z, value.w);
}

void Renderer::setShaderMat4(ShaderHandle handle, const char *name, const Mat4& value)
{
    assert(s_currentHandle == handle && "Shader must be active before changing values.");
    glUniformMatrix4fv(glGetUniformLocation(handle, name), 1, GL_TRUE, &value.data[0][0]);
}

void Renderer::setShaderTexture(ShaderHandle handle, const char *name, TextureHandle value, int slot)
{
    assert(s_currentHandle == handle && "Shader must be active before changing values.");

    // Load the texture into the requested slot.
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, value);

    // Point the uniform towards the slot we just filled.
    glUniform1i(glGetUniformLocation(handle, name), slot);

    // Clean up after ourselves.
    glActiveTexture(GL_TEXTURE0);
}
