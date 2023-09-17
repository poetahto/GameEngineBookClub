#pragma once

#include "resource_factory.hpp"
#include "resources/texture.hpp"

class TextureFactory final : public ResourceFactory
{
public:
    void serialize(const char* fileName, std::fstream& packageFile) override;
    bool canSerialize(const char* type) override;

    static void writeTexture(const Texture& texture, const char* fileName);
};

// namespace TextureFactory
// {
//     void writeTexture(const Texture& texture, const char* fileName);
//     Texture* readTexture(const char* fileName);
//
// } // namespace TextureFactory
