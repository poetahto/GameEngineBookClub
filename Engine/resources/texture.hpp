#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string_view>
#include "resource.hpp"
#include "types.hpp"
#include "binary_stream_builder.h"

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

    TextureWrapping wrappingX{TextureWrapping::Repeat};
    TextureWrapping wrappingY{TextureWrapping::Repeat};
    TextureFiltering textureFiltering{TextureFiltering::Bilinear};
    TextureFiltering mipmapFiltering{TextureFiltering::Bilinear};
    ColorFormat format{ColorFormat::Rgba};
    s32 width{};
    s32 height{};
    s32 channels{};
    u8* pixelData{};

    [[nodiscard]] s32 pixelCount() const
    {
        return width * height;
    }

    [[nodiscard]] s32 pixelDataLength() const
    {
        return pixelCount() * channels;
    }
};

template <typename T>
void writeResourceTo(T* resource, std::fstream& packageFile);

template <typename T>
T* readResourceFrom(std::fstream& packageFile);

// todo: i dont like the DRY here, maybe change to DataLayout builder that knows how to read + write from one definition?

template <>
inline void writeResourceTo<Texture>(Texture* resource, std::fstream& packageFile)
{
    BinaryStreamBuilder{&packageFile}
        .writeFixed(resource->wrappingX)
        .writeFixed(resource->wrappingY)
        .writeFixed(resource->textureFiltering)
        .writeFixed(resource->mipmapFiltering)
        .writeFixed(resource->format)
        .writeFixed(resource->width)
        .writeFixed(resource->height)
        .writeFixed(resource->channels)
        .write(resource->pixelData[0], resource->pixelDataLength());
}

template <>
inline Texture* readResourceFrom<Texture>(std::fstream& packageFile)
{
    Texture* resource = new Texture{};

    BinaryStreamBuilder builder{&packageFile};

    builder.readFixed(&resource->wrappingX)
           .readFixed(&resource->wrappingY)
           .readFixed(&resource->textureFiltering)
           .readFixed(&resource->mipmapFiltering)
           .readFixed(&resource->format)
           .readFixed(&resource->width)
           .readFixed(&resource->height)
           .readFixed(&resource->channels);

    resource->pixelData = new u8[resource->pixelDataLength()];
    builder.read(resource->pixelData, resource->pixelDataLength());
    return resource;
}


#endif // TEXTURE_HPP
