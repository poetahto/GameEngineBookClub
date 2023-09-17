#include "texture.hpp"
#include "logger.hpp"

const char* Texture::TYPE_NAME = "Texture";

s32 Texture::pixelCount() const
{
    return width * height;
}

s32 Texture::pixelDataLength() const
{
    return pixelCount() * channels;
}

template <>
void writeResourceTo<Texture>(Texture* resource, BinaryStreamBuilder& packageFile)
{
    packageFile.writeFixed(resource->wrappingX)
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
Texture* readResourceFrom<Texture>(BinaryStreamBuilder& packageFile)
{
    // todo: better allocation
    Texture* resource = new Texture{};

    packageFile.readFixed(&resource->wrappingX)
               .readFixed(&resource->wrappingY)
               .readFixed(&resource->textureFiltering)
               .readFixed(&resource->mipmapFiltering)
               .readFixed(&resource->format)
               .readFixed(&resource->width)
               .readFixed(&resource->height)
               .readFixed(&resource->channels);

    // todo: better allocation
    resource->pixelData = new u8[resource->pixelDataLength()];
    packageFile.read(resource->pixelData, resource->pixelDataLength());
    return resource;
}

// Texture Wrapping

const char* getDisplayName(TextureWrapping textureWrapping)
{
    if (textureWrapping == TextureWrapping::Repeat)
        return "Repeat";

    if (textureWrapping == TextureWrapping::MirroredRepeat)
        return "MirroredRepeat";

    if (textureWrapping == TextureWrapping::ClampEdge)
        return "ClampEdge";

    if (textureWrapping == TextureWrapping::ClampBorder)
        return "ClampBorder";

    Logger::log_error("Failed to parse display name for texture wrapping!");
    return nullptr;
}

TextureWrapping parseTextureWrapping(std::string_view value)
{
    if (value == "Repeat") return TextureWrapping::Repeat;
    if (value == "MirroredRepeat") return TextureWrapping::MirroredRepeat;
    if (value == "ClampEdge") return TextureWrapping::ClampEdge;
    if (value == "ClampBorder") return TextureWrapping::ClampBorder;

    Logger::log_error("Failed to parse texture wrapping from {}!", value);
    return TextureWrapping::Repeat;
}

// Texture Filtering

const char* getDisplayName(TextureFiltering textureFiltering)
{
    if (textureFiltering == TextureFiltering::Point)
        return "Point";

    if (textureFiltering == TextureFiltering::Bilinear)
        return "Bilinear";

    Logger::log_error("Failed to parse display name for texture filtering!");
    return nullptr;
}

TextureFiltering parseTextureFiltering(std::string_view value)
{
    if (value == "Point") return TextureFiltering::Point;
    if (value == "Bilinear") return TextureFiltering::Bilinear;

    Logger::log_error("Failed to parse texture filtering from {}!", value);
    return TextureFiltering::Bilinear;
}

// Color Format

const char* getDisplayName(ColorFormat colorFormat)
{
    if (colorFormat == ColorFormat::GrayScale)
        return "GrayScale";

    if (colorFormat == ColorFormat::GrayScaleAlpha)
        return "GrayScaleAlpha";

    if (colorFormat == ColorFormat::Rgb)
        return "Rgb";

    if (colorFormat == ColorFormat::Rgba)
        return "Rgba";

    Logger::log_error("Failed to parse display name for color format!");
    return nullptr;
}

ColorFormat parseColorFormat(std::string_view value)
{
    if (value == "GrayScale") return ColorFormat::GrayScale;
    if (value == "GrayScaleAlpha") return ColorFormat::GrayScaleAlpha;
    if (value == "Rgb") return ColorFormat::Rgb;
    if (value == "Rgba") return ColorFormat::Rgba;

    Logger::log_error("Failed to parse color format from {}!", value);
    return ColorFormat::Rgba;
}
