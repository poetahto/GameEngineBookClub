﻿#include "factory_util.hpp"
#include <fstream>
#include <iostream>

std::filesystem::path SETTINGS_FILE_DIR = "resources/settings";
std::filesystem::path BINARY_FILE_DIR = "resources/data";

std::fstream openFile(std::string_view fileName, std::ios::openmode openMode)
{
    std::fstream fileStream{};
    fileStream.open(fileName, openMode | std::ios::in | std::ios::out);

    if (!fileStream.is_open())
        std::cerr << "Failed to open file " << fileName << std::endl;

    return fileStream;
}

std::filesystem::path getSettingsFilePath(std::string_view fileName)
{
    std::filesystem::path path{fileName};
    return SETTINGS_FILE_DIR / path.filename() += ".json";
}

std::fstream openSettingsFile(std::string_view fileName, bool isWriting)
{
    std::ios::openmode openMode {0};

    if (isWriting)
        openMode |= std::ios::trunc;

    return openFile(getSettingsFilePath(fileName).string(), openMode);
}

std::filesystem::path getBinaryFilePath(std::string_view fileName)
{
    std::filesystem::path path{fileName};
    return BINARY_FILE_DIR / path.filename() += ".bin";
}

std::fstream openBinaryFile(std::string_view fileName, bool isWriting)
{
    std::ios::openmode openMode {std::ios::binary};

    if (isWriting)
        openMode |= std::ios::trunc;

    return openFile(getBinaryFilePath(fileName).string(), openMode);
}

void applyResourceSettings(Resource* resource, const nlohmann::json& settings)
{
    resource->name = settings["ResourceName"];
    resource->version = settings["ResourceVersion"];
    resource->type = settings["ResourceType"];
}

void writeResourceSettings(const Resource& resource, nlohmann::json& settings)
{
    settings["ResourceName"] = resource.name;
    settings["ResourceVersion"] = resource.version;
    settings["ResourceType"] = resource.type;
}
