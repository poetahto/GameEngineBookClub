#ifndef TEXTURE_H
#define TEXTURE_H

#include "renderer.h"

struct Texture
{
    Texture(const renderer::TextureData& data, const renderer::ImportSettings& settings);
    void free();
    renderer::TextureHandle handle;
};

#endif // TEXTURE_H
