#include "factory_util.h"
#include <fstream>
#include <iostream>

std::filesystem::path SETTINGS_FILE_DIR = "resources/settings";
std::filesystem::path BINARY_FILE_DIR = "resources/data";

std::fstream openFile(std::string_view fileName, std::ios::openmode openMode)
{
    std::fstream fileStream{};
    fileStream.open(fileName, openMode | std::ios::in | std::ios::out | std::ios::trunc);

    if (!fileStream.is_open())
        std::cerr << "Failed to open file " << fileName << std::endl;

    return fileStream;
}

std::filesystem::path getSettingsFilePath(std::string_view fileName)
{
    std::filesystem::path path{fileName};
    return SETTINGS_FILE_DIR / path.stem() += ".json";
}

std::fstream openSettingsFile(std::string_view fileName)
{
    return openFile(getSettingsFilePath(fileName).string(), 0);
}

std::filesystem::path getBinaryFilePath(std::string_view fileName)
{
    std::filesystem::path path{fileName};
    return BINARY_FILE_DIR / path.stem() += ".bin";
}

std::fstream openBinaryFile(std::string_view fileName)
{
    return openFile(getBinaryFilePath(fileName).string(), std::ios::binary);
}

void applyResourceSettings(Resource* resource, const nlohmann::json& settings)
{
    resource->name = settings["ResourceName"];
    resource->version = settings["ResourceVersion"];
}

void writeResourceSettings(const Resource& resource, nlohmann::json& settings)
{
    settings["ResourceName"] = resource.name;
    settings["ResourceVersion"] = resource.version;
}
