#ifndef TEXTURE_H
#define TEXTURE_H

#include "renderer.h"

struct Texture
{
    Texture(const Renderer::TextureData& data, const Renderer::ImportSettings& settings);
    void free();
    static Texture fromFile(const char* file, const Renderer::ImportSettings& settings);

    Renderer::TextureHandle handle;
    Renderer::TextureData data;
    Renderer::ImportSettings importSettings;
};

#endif // TEXTURE_H
