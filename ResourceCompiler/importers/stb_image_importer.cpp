#include "stb_image_importer.hpp"
#include "resources/texture.hpp"
#include "../factories/texture_factory.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::unordered_set<std::string_view> StbImageImporter::supportedExtensions()
{
    return {".png", ".ppm", ".jpg", ".jpeg", ".tga", ".hdr", ".bmp"};
}

bool StbImageImporter::process(const char* fileName)
{
    Texture texture{};

    texture.name = fileName;
    texture.version = 1;
    texture.pixelData = stbi_load(fileName, &texture.width, &texture.height, &texture.channels, 0);

    if (texture.pixelData == nullptr)
        return false;

    // Converting the stb color channel to our format enum.
    {
        if (texture.channels == 1)
            texture.format = ColorFormat::GrayScale;

        else if (texture.channels == 2)
            texture.format = ColorFormat::GrayScaleAlpha;

        else if (texture.channels == 3)
            texture.format = ColorFormat::Rgb;

        else if (texture.channels == 4)
            texture.format = ColorFormat::Rgba;
    }

    TextureFactory::writeTexture(texture, fileName);
    return true;
}
