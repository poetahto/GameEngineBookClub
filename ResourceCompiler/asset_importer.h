#pragma once

#include <string_view>
#include <unordered_set>

class AssetImporter
{
public:
    virtual ~AssetImporter() = default;
    virtual std::unordered_set<std::string_view> supportedExtensions() = 0;
    virtual bool process(const char* fileName) = 0;
};
