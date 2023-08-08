#ifndef SHADER_H
#define SHADER_H

#include "renderer.h"

struct Texture;

struct Shader
{
    typedef s32 Location;

    explicit Shader(const char* vertexSource, const char* fragmentSource);
    static Shader fromFiles(const char* vertexSourceFile, const char* fragmentSourceFile);
    static Shader fromSingleFile(const char* combinedSourceFile);
    void free();
    void use() const;
    static void clear();

    void setFloat(const char* name, f32 value);
    void setDouble(const char* name, f64 value);
    void setInt(const char* name, s32 value);
    void setBool(const char* name, bool value);
    void setVec3(const char* name, const Vec3& value);
    void setMat4(const char* name, const Mat4& value);
    void setTexture(const char* name, const Texture& value, int slot = 0);
    renderer::ShaderHandle handle;
};

#endif
