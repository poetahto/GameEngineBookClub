#ifndef RENDERER_H
#define RENDERER_H

#include "containers/fixed_list.h"

struct Mat4;
struct Vec3;
struct Vec4;

namespace renderer
{
    typedef u32 MeshHandle;
    typedef u32 TextureHandle;
    typedef u32 ShaderHandle;

    typedef FixedList<f32> VertexList;
    typedef FixedList<s32> VertexFormat;
    typedef FixedList<u32> IndexList;

    struct TextureData
    {
        s32 width;
        s32 height;
        const void* data;
    };

    struct ImportSettings
    {
        enum Format { R, Rgb, Rgba, Rgba8 };
        enum Wrapping { Repeat, MirroredRepeat, ClampEdge, ClampBorder };
        enum Filtering { Point, Bilinear };

        Format format{Rgba};
        Wrapping wrappingX{Repeat};
        Wrapping wrappingY{Repeat};
        Filtering textureFiltering{Bilinear};
        Filtering mipmapFiltering{Bilinear};
    };

    // General Functions.

    void initialize(s32 width, s32 height);
    void clearScreen(f32 red, f32 green, f32 blue);
    void clearScreen(Vec3 color);

    // Texture functions.

    TextureHandle uploadTexture(const TextureData& data, const ImportSettings& settings);
    void deleteTexture(TextureHandle handle);

    // Mesh functions.

    MeshHandle uploadMesh(VertexList vertices, VertexFormat format, IndexList indices);
    void deleteMesh(MeshHandle handle);
    void drawMesh(MeshHandle handle);

    // Shader functions.

    ShaderHandle uploadShader(const char* vertexSource, const char* fragmentSource);
    void deleteShader(ShaderHandle handle);
    void useShader(ShaderHandle handle);
    void clearShaders();
    void setShaderFloat(ShaderHandle handle, const char* name, f32 value);
    void setShaderDouble(ShaderHandle handle, const char* name, f64 value);
    void setShaderInt(ShaderHandle handle, const char* name, s32 value);
    void setShaderBool(ShaderHandle handle, const char* name, bool value);
    void setShaderVec3(ShaderHandle handle, const char* name, const Vec3& value);
    void setShaderVec4(ShaderHandle handle, const char* name, const Vec4& value);
    void setShaderMat4(ShaderHandle handle, const char* name, const Mat4& value);
    void setShaderTexture(ShaderHandle handle, const char* name, TextureHandle value, int slot = 0);

} // namespace renderer

#endif // RENDERER_H
