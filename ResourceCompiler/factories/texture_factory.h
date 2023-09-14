#pragma once

#include "resources/texture.hpp"

namespace TextureFactory
{
    void writeTexture(const Texture& texture, const char* fileName);
    Texture* readTexture(const char* fileName);

} // namespace TextureFactory
