#ifndef SHADER_H
#define SHADER_H

#include "renderer.hpp"

struct Vec2;
struct Texture;

struct Shader
{
    typedef s32 Location;

    explicit Shader(const char* vertexSource, const char* fragmentSource);
    static Shader fromMaterial(const char* materialFile);
    static Shader fromFiles(const char* vertexSourceFile, const char* fragmentSourceFile);
    static Shader fromSingleFile(const char* combinedSourceFile);
    void free();
    void use() const;
    static void clear();

    void setFloat(const char* name, f32 value);
    void setDouble(const char* name, f64 value);
    void setInt(const char* name, s32 value);
    void setBool(const char* name, bool value);
    void setVec2(const char* name, const Vec2& value);
    void setVec3(const char* name, const Vec3& value);
    void setVec4(const char* name, const Vec4& value);
    void setMat4(const char* name, const Mat4& value);
    void setTexture(const char* name, const Texture& value, int slot = 0);
    Renderer::ShaderHandle handle;
};

#endif // SHADER_H
