#ifndef RENDERER_H
#define RENDERER_H

#include "containers/fixed_list.h"
#include "platform/application.h"

struct Shader;
struct Vec2;
struct Mat4;
struct Vec3;
struct Vec4;

// todo: not really happy w/ how this renderer platform layer came out, I feel like its too high level.
// expose more low-level gd stuff, reference other abstraction libraries (sokol, veldrid)
namespace Renderer
{
    typedef u32 MeshHandle;
    typedef u32 TextureHandle;
    typedef u32 ShaderHandle;

    typedef FixedList<f32> VertexList;
    typedef FixedList<s32> VertexFormat;
    typedef FixedList<u32> IndexList;

    struct TextureData
    {
        enum Format { R, Rgb, Rgba, Rgba8 };

        Format format{Rgba};
        s32 width;
        s32 height;
        s32 stride;
        u8* data;

        template <typename T>
        T* get(s32 x, s32 y) const
        {
            s32 index = y * (width * stride) + x * stride;
            return reinterpret_cast<T*>(&data[index]);
        }

        s32 getPixels() const;
        s32 getDataLength() const;
    };

    struct ImportSettings
    {
        enum Wrapping { Repeat, MirroredRepeat, ClampEdge, ClampBorder };

        enum Filtering { Point, Bilinear };

        Wrapping wrappingX{Repeat};
        Wrapping wrappingY{Repeat};
        Filtering textureFiltering{Bilinear};
        Filtering mipmapFiltering{Bilinear};

        static ImportSettings fromFile(const char* fileName);
    };

    // General Functions.

    void init(s32 width = Application::Settings::DEFAULT_WIDTH, s32 height = Application::Settings::DEFAULT_HEIGHT);
    void resize(s32 width, s32 height);
    void clearScreen(f32 red, f32 green, f32 blue);
    void clearScreen(Vec3 color);

    // Texture functions.

    TextureHandle uploadTexture(const TextureData& data, const ImportSettings& settings);
    void deleteTexture(TextureHandle handle);

    // Mesh functions.

    enum DrawMode
    {
        Triangles,
        TriangleStrips
    };

    MeshHandle uploadMesh(VertexList vertices, VertexFormat format, IndexList indices);
    void deleteMesh(MeshHandle handle);
    void drawMesh(MeshHandle handle, DrawMode mode);

    // Shader functions.

    ShaderHandle uploadShader(const char* vertexSource, const char* fragmentSource);
    void deleteShader(ShaderHandle handle);
    void useShader(ShaderHandle handle);
    void clearShaders();

    void setShaderFloat(ShaderHandle handle, const char* name, f32 value);
    void setShaderDouble(ShaderHandle handle, const char* name, f64 value);
    void setShaderInt(ShaderHandle handle, const char* name, s32 value);
    void setShaderBool(ShaderHandle handle, const char* name, bool value);
    void setShaderVec2(ShaderHandle handle, const char* name, const Vec2& value);
    void setShaderVec3(ShaderHandle handle, const char* name, const Vec3& value);
    void setShaderVec4(ShaderHandle handle, const char* name, const Vec4& value);
    void setShaderMat4(ShaderHandle handle, const char* name, const Mat4& value);
    void setShaderTexture(ShaderHandle handle, const char* name, TextureHandle value, int slot = 0);
} // namespace renderer

#endif // RENDERER_H
