﻿#include "texture_factory.h"
#include "factory_util.h"
#include "binary_stream_builder.h"
#include <nlohmann/json.hpp>

void TextureFactory::writeTexture(const Texture& texture, const char* fileName)
{
    nlohmann::json settings{};
    writeResourceSettings(texture, settings);
    settings["MipmapFiltering"] = getDisplayName(texture.mipmapFiltering);
    settings["TextureFiltering"] = getDisplayName(texture.textureFiltering);
    settings["WrappingX"] = getDisplayName(texture.wrappingX);
    settings["WrappingY"] = getDisplayName(texture.wrappingY);

    std::fstream settingsFile = openSettingsFile(fileName);
    settingsFile << std::setw(4) << settings;
    settingsFile.close();

    std::fstream binaryFile = openBinaryFile(fileName);

    BinaryStreamBuilder{&binaryFile}
        .writeFixed(texture.width)
        .writeFixed(texture.height)
        .writeFixed(texture.format)
        .writeFixed(texture.channels)
        .write(texture.pixelData[0], texture.pixelDataLength());

    binaryFile.close();
}

Texture* readTexture(const char* fileName)
{
    Texture* texture = new Texture{};
    nlohmann::json settings{};

    std::fstream settingsFile = openSettingsFile(fileName);
    settingsFile >> settings;
    settingsFile.close();

    applyResourceSettings(texture, settings);
    texture->mipmapFiltering = parseTextureFiltering(settings["MipmapFiltering"]);
    texture->textureFiltering = parseTextureFiltering(settings["TextureFiltering"]);
    texture->wrappingX = parseTextureWrapping(settings["WrappingX"]);
    texture->wrappingY = parseTextureWrapping(settings["WrappingY"]);

    std::fstream binaryFile = openBinaryFile(fileName);

    BinaryStreamBuilder{&binaryFile}
        .readFixed(&texture->width)
        .readFixed(&texture->height)
        .readFixed(&texture->format)
        .readFixed(&texture->channels)
        .read(&texture->pixelData, texture->pixelDataLength());

    binaryFile.close();

    return texture;
}
