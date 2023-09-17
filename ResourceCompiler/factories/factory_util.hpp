#pragma once

#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "resources/resource.hpp"

extern std::filesystem::path SETTINGS_FILE_DIR;
extern std::filesystem::path BINARY_FILE_DIR;

std::fstream openFile(std::string_view fileName, std::ios::openmode openMode);

std::filesystem::path getSettingsFilePath(std::string_view fileName);
std::fstream openSettingsFile(std::string_view fileName, bool isWriting);

std::filesystem::path getBinaryFilePath(std::string_view fileName);
std::fstream openBinaryFile(std::string_view fileName, bool isWriting);

void applyResourceSettings(Resource* resource, const nlohmann::json& settings);
void writeResourceSettings(const Resource& resource, nlohmann::json& settings);
