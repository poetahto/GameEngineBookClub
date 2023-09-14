#include "shader.hpp"

#include <unordered_map>

#include "../../Engine/string_util.hpp"
#include "texture.hpp"
#include "../../Engine/math/vec3.hpp"

Shader::Shader(const char* vertexSource, const char* fragmentSource) :
    handle{Renderer::uploadShader(vertexSource, fragmentSource)}
{
}

f32 parseFloat(std::stringstream& stream, char delimiter)
{
    std::string stringValue;
    std::getline(stream, stringValue, delimiter);
    return std::stof(stringValue);
}

void vec3PropertyApplier(const char* name, const std::string& rawValue, Shader shader)
{
    std::stringstream stream{rawValue};

    Vec3 result
    {
        parseFloat(stream, ' '),
        parseFloat(stream, ' '),
        parseFloat(stream, '\n')
    };

    shader.setVec3(name, result);
}

void floatPropertyApplier(const char* name, const std::string& rawValue, Shader shader)
{
    f32 result = std::stof(rawValue);
    shader.setFloat(name, result);
}

void texturePropertyApplier(const char* name, const std::string& rawValue, Shader shader)
{
    std::stringstream stream {rawValue};

    std::string textureFile;
    std::getline(stream, textureFile, ' ');

    std::string importFile;
    std::getline(stream, importFile, ' ');
    Renderer::ImportSettings settings = Renderer::ImportSettings::fromFile(importFile.c_str());

    Texture texture = Texture::fromFile(textureFile.c_str(), settings);

    std::string textureIndex;
    std::getline(stream, textureIndex);
    s32 index = std::stoi(textureIndex);

    shader.setTexture(name, texture, index);
}

typedef void (*MaterialPropertyApplier)(const char* name, const std::string& rawInput, Shader shader);

std::unordered_map<std::string, MaterialPropertyApplier> s_applierTable
{
    {
        {"float", floatPropertyApplier},
        {"vec3", vec3PropertyApplier},
        {"texture", texturePropertyApplier},
    }
};

// todo: this is fundamentally broken, cus property stuff needs to be reset before drawing every time.
Shader Shader::fromMaterial(const char* materialFile)
{
    /* Example material file:
     * [vert shader]
     * [frag shader]
     * [type] [name] [value]
     * ...
     * test.frag
     * test.vert
     * vec3 color 1 1 1
     * float intensity 0.5
     * texture albedo albedo.png default.teximport 1 // todo: i see the need for a nice ACP now...
     */

    std::ifstream stream{};
    stream.open(materialFile);

    if (!stream.is_open())
        printf("failed to open material %s", materialFile);

    std::string vertShaderFile;
    std::getline(stream, vertShaderFile);

    std::string fragShaderFile;
    std::getline(stream, fragShaderFile);

    Shader result = fromFiles(vertShaderFile.c_str(), fragShaderFile.c_str());
    result.use();

    while (stream.peek() != EOF)
    {
        std::string type;
        std::getline(stream, type, ' ');

        std::string name;
        std::getline(stream, name, ' ');

        std::string rawInput;
        std::getline(stream, rawInput, '\n');

        s_applierTable[type](name.c_str(), rawInput, result);
    }

    return result;
}

Shader Shader::fromFiles(const char* vertexSourceFile, const char* fragmentSourceFile)
{
    std::string vertexInput{readFromFile(vertexSourceFile)};
    std::string fragmentInput{readFromFile(fragmentSourceFile)};

    return Shader{vertexInput.c_str(), fragmentInput.c_str()};
}

Shader Shader::fromSingleFile(const char* combinedSourceFile)
{
    std::string input = readFromFile(combinedSourceFile);
    std::string vertexSource{parseBetweenTags(input, "<vertex>", "</vertex>")};
    std::string fragmentSource{parseBetweenTags(input, "<fragment>", "</fragment>")};

    return Shader{vertexSource.c_str(), fragmentSource.c_str()};
}

void Shader::free()
{
    Renderer::deleteShader(handle);
}

void Shader::use() const
{
    Renderer::useShader(handle);
}

void Shader::setFloat(const char* name, float value)
{
    Renderer::setShaderFloat(handle, name, value);
}

void Shader::setDouble(const char* name, f64 value)
{
    Renderer::setShaderDouble(handle, name, value);
}

void Shader::setInt(const char* name, int value)
{
    Renderer::setShaderInt(handle, name, value);
}

void Shader::setBool(const char* name, bool value)
{
    Renderer::setShaderBool(handle, name, value);
}

void Shader::setVec2(const char* name, const Vec2& value)
{
    Renderer::setShaderVec2(handle, name, value);
}

void Shader::setVec3(const char* name, const Vec3& value)
{
    Renderer::setShaderVec3(handle, name, value);
}

void Shader::setVec4(const char* name, const Vec4& value)
{
    Renderer::setShaderVec4(handle, name, value);
}

void Shader::setMat4(const char* name, const Mat4& value)
{
    Renderer::setShaderMat4(handle, name, value);
}

void Shader::setTexture(const char* name, const Texture& value, int slot)
{
    Renderer::setShaderTexture(handle, name, value.handle, slot);
}

void Shader::clear()
{
    Renderer::clearShaders();
}
