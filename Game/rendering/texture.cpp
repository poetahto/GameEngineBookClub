#include "texture.h"

#include <fstream>
#include <string>

Texture::Texture(const Renderer::TextureData& data, const Renderer::ImportSettings& settings) :
    handle {uploadTexture(data, settings) },
    data {data},
    importSettings{settings}
{
}

Texture::Texture(const Renderer::TextureData& data) :
    handle {uploadTexture(data, Renderer::ImportSettings{})},
    data {data},
    importSettings({Renderer::ImportSettings{}})
{
}

void Texture::free()
{
    Renderer::deleteTexture(handle);
}

Texture Texture::fromFile(const char* file, const Renderer::ImportSettings& settings)
{
    std::ifstream fileStream {};
    fileStream.open(file, std::ifstream::in | std::ifstream::binary);

    if (!fileStream.is_open())
        printf("Failed to open texture from file %s", file);

    char buffer[500] {};
    Renderer::TextureData data;
    data.format = Renderer::TextureData::Rgb;

    fileStream.seekg(3); // skip the magic number?

    fileStream.getline(buffer, 20, ' ');
    data.width = std::stoi(buffer);

    fileStream.getline(buffer, 20);
    data.height = std::stoi(buffer);

    fileStream.getline(buffer, 20); // skip the format?

    data.stride = 3;
    char* dataBuffer = new char[data.getDataLength()] {};
    fileStream.read(dataBuffer, data.getDataLength());
    data.data = reinterpret_cast<u8*>(dataBuffer);

    return Texture{data, settings};
}

Texture Texture::fromFile(const char* file)
{
    auto settings = Renderer::ImportSettings{};
    return fromFile(file, settings);
}
