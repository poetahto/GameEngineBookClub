#pragma once

#include "../resource_importer.hpp"

class StbImageImporter final : public ResourceImporter
{
public:
    std::unordered_set<std::string_view> supportedExtensions() override;
    bool process(const char* fileName) override;
};
