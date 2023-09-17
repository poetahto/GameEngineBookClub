#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "logger.hpp"
#include "resource_factory.hpp"
#include "resource_importer.hpp"
#include "string_name.hpp"
#include "factories/factory_util.hpp"
#include "factories/texture_factory.hpp"
#include "importers/stb_image_importer.hpp"
#include "math/vec3.hpp"

using namespace std;
using namespace std::filesystem;

int build(const path& sourceDir, const path& buildDir);
int package(const path& buildDir, const path& packageDir);

int main(int argc, char* argv[])
{
    if (argc >= 4)
    {
        if (strcmp(argv[1], "build") == 0)
        {
            return build(argv[2], argv[3]);
        }

        if (strcmp(argv[1], "package") == 0)
        {
            return package(argv[2], argv[3]);
        }
    }

    cerr << "usage: \nbuild [sourceDir] [buildDir]\npackage [buildDir] [packageDir]\n";
    return 1;
}

int build(const path& sourceDir, const path& buildDir)
{
    SETTINGS_FILE_DIR = buildDir / "settings";
    BINARY_FILE_DIR = buildDir / "data";

    cout << "source: " << sourceDir << ", build: " << buildDir << endl;

    vector<ResourceImporter*> importers{
        new StbImageImporter{},
    };

    for (path file : recursive_directory_iterator{sourceDir})
    {
        for (ResourceImporter* importer : importers)
        {
            if (importer->supportedExtensions().contains(file.extension().string()))
            {
                cout << "converting " << file << std::endl;
                importer->process(file.string().c_str());
            }
        }
    }

    return 0;
}

int package(const path& buildDir, const path& packageDir)
{
    SETTINGS_FILE_DIR = buildDir / "settings";
    BINARY_FILE_DIR = buildDir / "data";

    std::vector<ResourceFactory*> factories{
        new TextureFactory{},
    };

    // desired output: one big package file. header = resource name hashes -> resource offset
    // step 1: calculate header size (key-values for each resource)
    //   - stringHash size (u64)
    //   - resource offset (size_t)
    //   - total header size = sizeof(stringHash + size_t struct) * resource files

    std::unordered_map<StringName::Hash, size_t> hashToOffset;
    size_t assetCount{};

    for (directory_entry entry : recursive_directory_iterator{SETTINGS_FILE_DIR})
        assetCount++;

    size_t headerSize{(sizeof(StringName::Hash) + sizeof(size_t)) * assetCount + sizeof(size_t)};
    Logger::log("{} bytes reserved for header.", headerSize);

    // step 2: loop through all settings + data files and create memory reps, and append to file (storing name hash -> pos in dict)
    //   - factories[RESOURCE_TYPE]->serialize(const char* fileName)

    std::fstream packageFile{};
    packageFile.open(packageDir / "resources.pak", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    BinaryStreamBuilder packageBuilder{&packageFile};
    packageFile.seekg(static_cast<std::streamoff>(headerSize));

    if (!packageFile.is_open())
        Logger::log_error("Failed to open package file!");

    for (const directory_entry& entry : recursive_directory_iterator{SETTINGS_FILE_DIR})
    {
        nlohmann::json settings{};
        std::fstream settingsFile{};
        settingsFile.open(entry.path());
        settingsFile >> settings;
        settingsFile.close();

        for (ResourceFactory* factory : factories)
        {
            std::string type = settings["ResourceType"];
            std::string name = settings["ResourceName"];

            if (factory->canSerialize(type))
            {
                Logger::log("{} is being serialized", entry.path().string());
                StringName::Hash hash{StringName::createHash(name.c_str())};
                hashToOffset.emplace(hash, packageFile.tellg());
                factory->serialize(entry.path().stem().string(), packageBuilder);
            }
        }
    }

    // step 3: insert the generated dict into the reserved header
    packageFile.seekg(0);
    packageBuilder.writeFixed(assetCount);

    for (auto [hash, offset] : hashToOffset)
    {
        Logger::log("Hash: {}, Offset: {}", hash, offset);

        packageBuilder.writeFixed(hash)
                      .writeFixed(offset);
    }

    packageFile.close();
    return 0;
}
