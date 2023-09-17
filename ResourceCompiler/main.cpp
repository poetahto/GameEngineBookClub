#include <iostream>
#include <fstream>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <filesystem>
#include "math/vec3.hpp"
#include "resource_importer.hpp"
#include "logger.hpp"
#include "resource_factory.hpp"
#include "factories/factory_util.h"
#include "importers/stb_image_importer.h"
#include "string_name.hpp"
#include "factories/texture_factory.h"

using namespace std;
using namespace std::filesystem;

int main(int argc, char* argv[])
{
    // One arguments = build dir, build the package file
    if (argc == 2)
    {
        path buildDir{argv[1]};
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
        packageFile.open(buildDir / "resources.pak", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
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

                if (factory->canSerialize(type.c_str()))
                {
                    Logger::log("{} is being serialized", entry.path().string());
                    StringName::Hash hash{StringName::createHash(name.c_str())};
                    hashToOffset.emplace(hash, packageFile.tellg());
                    factory->serialize(entry.path().stem().string().c_str(), packageFile);
                }
            }
        }

        // step 3: insert the generated dict into the reserved header
        packageFile.seekg(0);
        BinaryStreamBuilder builder{&packageFile};
        builder.writeFixed(assetCount);

        for (auto [hash, offset] : hashToOffset)
        {
            Logger::log("Hash: {}, Offset: {}", hash, offset);

            builder.writeFixed(hash)
                   .writeFixed(offset);
        }

        packageFile.close();

        return 0;
    }

    // Two arguments = source and build dir, create resource files
    if (argc == 3)
    {
        path sourceDir{argv[1]};
        path buildDir{argv[2]};
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

    cerr << "usage: [source dir] [build dir]";
    return 1;
}
