#pragma once

#include <string_view>

class BinaryStreamBuilder;

class ResourceFactory
{
public:
    virtual ~ResourceFactory() = default;
    virtual void serialize(std::string_view fileName, BinaryStreamBuilder& packageFile) = 0;
    virtual bool canSerialize(std::string_view type) = 0;
};
