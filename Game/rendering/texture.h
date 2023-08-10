#ifndef TEXTURE_H
#define TEXTURE_H

#include "renderer.h"

struct Texture
{
    Texture(const renderer::TextureData& data, const renderer::ImportSettings& settings);
    void free();
    static Texture fromFile(const char* file, const renderer::ImportSettings& settings);

    renderer::TextureHandle handle;
    renderer::TextureData data;
    renderer::ImportSettings importSettings;
};

#endif // TEXTURE_H
