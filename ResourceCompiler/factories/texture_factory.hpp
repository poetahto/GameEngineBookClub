#pragma once

#include "resource_factory.hpp"
#include "resources/texture.hpp"

class TextureFactory final : public ResourceFactory
{
public:
    void serialize(std::string_view fileName, BinaryStreamBuilder& packageFile) override;
    bool canSerialize(std::string_view type) override;

    static void writeTexture(const Texture& texture, const char* fileName);
};
