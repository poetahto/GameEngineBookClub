#pragma once

#include "asset_importer.h"

class StbImageImporter final : public AssetImporter
{
public:
    std::unordered_set<std::string_view> supportedExtensions() override;
    bool process(const char* fileName) override;
};
