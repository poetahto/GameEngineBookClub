#include "texture.h"

#include <fstream>
#include <string>

Texture::Texture(const renderer::TextureData& data, const renderer::ImportSettings& settings) :
    handle {uploadTexture(data, settings) }
{
}

void Texture::free()
{
    renderer::deleteTexture(handle);
}

Texture Texture::fromFile(const char* file, const renderer::ImportSettings& settings)
{
    std::ifstream fileStream {};
    fileStream.open(file, std::ifstream::in | std::ifstream::binary);

    if (!fileStream.is_open())
        printf("Failed to open texture from file %s", file);

    char buffer[500] {};
    renderer::TextureData data;
    data.format = renderer::TextureData::Rgb;

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
