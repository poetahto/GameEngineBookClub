#include "shader.h"
#include "string_util.h"
#include "texture.h"

Shader::Shader(const char* vertexSource, const char* fragmentSource) :
    handle {renderer::uploadShader(vertexSource, fragmentSource) }
{
}

Shader Shader::fromFiles(const char* vertexSourceFile, const char* fragmentSourceFile)
{
    std::string vertexInput { readFromFile(vertexSourceFile) };
    std::string fragmentInput { readFromFile(fragmentSourceFile) };

    return Shader {vertexInput.c_str(), fragmentInput.c_str()};
}

Shader Shader::fromSingleFile(const char* combinedSourceFile)
{
    std::string input = readFromFile(combinedSourceFile);
    std::string vertexSource { parseBetweenTags(input, "<vertex>", "</vertex>") };
    std::string fragmentSource { parseBetweenTags(input, "<fragment>", "</fragment>") };

    return Shader {vertexSource.c_str(), fragmentSource.c_str()};
}

void Shader::free()
{
    renderer::deleteShader(handle);
}

void Shader::use() const
{
    renderer::useShader(handle);
}

void Shader::setFloat(const char *name, float value)
{
    renderer::setShaderFloat(handle, name, value);
}

void Shader::setDouble(const char* name, f64 value)
{
    renderer::setShaderDouble(handle, name, value);
}

void Shader::setInt(const char *name, int value)
{
    renderer::setShaderInt(handle, name, value);
}

void Shader::setBool(const char *name, bool value)
{
    renderer::setShaderBool(handle, name, value);
}

void Shader::setVec2(const char* name, const Vec2& value)
{
    renderer::setShaderVec2(handle, name, value);
}

void Shader::setVec3(const char *name, const Vec3& value)
{
    renderer::setShaderVec3(handle, name, value);
}

void Shader::setVec4(const char *name, const Vec4& value)
{
    renderer::setShaderVec4(handle, name, value);
}

void Shader::setMat4(const char *name, const Mat4& value)
{
    renderer::setShaderMat4(handle, name, value);
}

void Shader::setTexture(const char *name, const Texture& value, int slot)
{
    renderer::setShaderTexture(handle, name, value.handle, slot);
}

void Shader::clear()
{
    renderer::clearShaders();
}
