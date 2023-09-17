#ifndef TEXTURE_H
#define TEXTURE_H

#include "renderer.hpp"

struct GameTexture
{
    explicit GameTexture(const Texture* data);
    void free();

    Renderer::TextureHandle handle;
    const Texture* texture;
};

#endif // TEXTURE_H
