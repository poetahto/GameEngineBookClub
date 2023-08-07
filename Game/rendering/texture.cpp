#include "texture.h"

Texture::Texture(const renderer::TextureData& data, const renderer::ImportSettings& settings) :
    handle {uploadTexture(data, settings) }
{
}

void Texture::free()
{
    renderer::deleteTexture(handle);
}
