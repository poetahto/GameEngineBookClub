#include "texture_factory.hpp"
#include "factory_util.hpp"
#include "../../Engine/resources/binary_stream_builder.hpp"
#include <nlohmann/json.hpp>

void TextureFactory::serialize(std::string_view fileName, BinaryStreamBuilder& packageFile)
{
    Texture* texture = new Texture{};

    std::fstream settingsFile = openSettingsFile(fileName, false);
    nlohmann::json settings = nlohmann::json::parse(settingsFile);
    settingsFile.close();

    applyResourceSettings(texture, settings);
    texture->mipmapFiltering = parseTextureFiltering(settings["TextureMipmapFiltering"]);
    texture->textureFiltering = parseTextureFiltering(settings["TextureFiltering"]);
    texture->wrappingX = parseTextureWrapping(settings["TextureWrappingX"]);
    texture->wrappingY = parseTextureWrapping(settings["TextureWrappingY"]);

    std::fstream binaryFile = openBinaryFile(fileName, false);
    BinaryStreamBuilder builder{&binaryFile};

    builder.readFixed(&texture->width)
           .readFixed(&texture->height)
           .readFixed(&texture->format)
           .readFixed(&texture->channels);

    texture->pixelData = new u8[texture->pixelDataLength()]{};
    builder.read(texture->pixelData, texture->pixelDataLength());
    binaryFile.close();

    writeResourceTo(texture, packageFile);
}

void TextureFactory::writeTexture(const Texture& texture, const char* fileName)
{
    nlohmann::json settings{};
    writeResourceSettings(texture, settings);
    settings["TextureMipmapFiltering"] = getDisplayName(texture.mipmapFiltering);
    settings["TextureFiltering"] = getDisplayName(texture.textureFiltering);
    settings["TextureWrappingX"] = getDisplayName(texture.wrappingX);
    settings["TextureWrappingY"] = getDisplayName(texture.wrappingY);

    std::fstream settingsFile = openSettingsFile(fileName, true);
    settingsFile << std::setw(4) << settings;
    settingsFile.close();

    std::fstream binaryFile = openBinaryFile(fileName, true);

    BinaryStreamBuilder{&binaryFile}
        .writeFixed(texture.width)
        .writeFixed(texture.height)
        .writeFixed(texture.format)
        .writeFixed(texture.channels)
        .write(texture.pixelData[0], texture.pixelDataLength());

    binaryFile.close();
}

bool TextureFactory::canSerialize(std::string_view type)
{
    return type == Texture::TYPE_NAME;
}
