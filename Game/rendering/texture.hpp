#ifndef TEXTURE_H
#define TEXTURE_H

#include "renderer.hpp"

struct GameTexture
{
    GameTexture(const Renderer::TextureData& data, const Renderer::ImportSettings& settings);
    explicit GameTexture(const Renderer::TextureData& data);
    void free();
    static GameTexture fromFile(const char* file, const Renderer::ImportSettings& settings);
    static GameTexture fromFile(const char* file);

    Renderer::TextureHandle handle;
    Renderer::TextureData data;
    Renderer::ImportSettings importSettings;
};

#endif // TEXTURE_H
