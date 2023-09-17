#include "game_texture.hpp"

GameTexture::GameTexture(const Texture* data) :
    handle{Renderer::uploadTexture(*data)},
    texture{data}
{
}

void GameTexture::free()
{
    Renderer::deleteTexture(handle);
}
