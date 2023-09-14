#include "texture.hpp"
#include "logger.hpp"

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
