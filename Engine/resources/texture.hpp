#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string_view>
#include "resource.hpp"
#include "types.hpp"

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
    Texture()
    {
        type = "Texture";
    }

    TextureWrapping wrappingX {TextureWrapping::Repeat};
    TextureWrapping wrappingY {TextureWrapping::Repeat};
    TextureFiltering textureFiltering {TextureFiltering::Bilinear};
    TextureFiltering mipmapFiltering {TextureFiltering::Bilinear};
    ColorFormat format {ColorFormat::Rgba};
    s32 width {};
    s32 height {};
    s32 channels {};
    u8* pixelData {};

    [[nodiscard]] s32 pixelCount() const
    {
        return width * height;
    }

    [[nodiscard]] s32 pixelDataLength() const
    {
        return pixelCount() * channels;
    }
};

#endif // TEXTURE_HPP
