#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string_view>
#include "resource.hpp"
#include "types.hpp"
#include "binary_stream_builder.hpp"
#include "resource_manager.hpp"

enum class ColorFormat { GrayScale, GrayScaleAlpha, Rgb, Rgba };

const char* getDisplayName(ColorFormat colorFormat);
ColorFormat parseColorFormat(std::string_view value);

enum class TextureFiltering { Point, Bilinear };

const char* getDisplayName(TextureFiltering textureFiltering);
TextureFiltering parseTextureFiltering(std::string_view value);

enum class TextureWrapping { Repeat, MirroredRepeat, ClampEdge, ClampBorder };

const char* getDisplayName(TextureWrapping textureWrapping);
TextureWrapping parseTextureWrapping(std::string_view value);

struct Texture : Resource
{
    static const char* TYPE_NAME;

    Texture()
    {
        type = TYPE_NAME;
    }

    TextureWrapping wrappingX{TextureWrapping::Repeat};
    TextureWrapping wrappingY{TextureWrapping::Repeat};
    TextureFiltering textureFiltering{TextureFiltering::Bilinear};
    TextureFiltering mipmapFiltering{TextureFiltering::Bilinear};
    ColorFormat format{ColorFormat::Rgba};
    s32 width{};
    s32 height{};
    s32 channels{};
    u8* pixelData{};

    template <typename T>
    T* get(s32 x, s32 y) const;

    [[nodiscard]] s32 pixelCount() const;
    [[nodiscard]] s32 pixelDataLength() const;
};

template <typename T>
T* Texture::get(s32 x, s32 y) const
{
    s32 index = y * (width * channels) + x * channels;
    return reinterpret_cast<T*>(&pixelData[index]);
}

template <>
void writeResourceTo<Texture>(Texture* resource, BinaryStreamBuilder& packageFile);

template <>
Texture* readResourceFrom<Texture>(BinaryStreamBuilder& packageFile);

#endif // TEXTURE_HPP
