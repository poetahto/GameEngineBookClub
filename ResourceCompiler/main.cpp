#include <iostream>
#include <fstream>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <filesystem>
#include "math/vec3.hpp"
#include "resources/texture.hpp"
#include "asset_importer.h"
#include "factories/factory_util.h"
#include "importers/stb_image_importer.h"

using namespace std;
using namespace std::filesystem;

int main(int argc, char* argv[])
{
    // One arguments = build dir, build the package file
    if (argc == 2)
    {
        path buildDir {argv[1]};
        path settingsDir {buildDir / "settings"};
        path binaryDir {buildDir / "data"};

        // desired output: one big package file. header = resource name hashes -> resource offset
        // step 1: calculate header size (key-values for each resource)
        //   - stringHash size (u64)
        //   - resource offset (size_t)
        //   - total header size = sizeof(stringHash + size_t struct) * resource files
        // step 2: loop through all settings + data files and create memory reps, and append to file (storing name hash -> pos in dict)
        //   - factories[RESOURCE_TYPE]->serialize(const char* fileName)
        // step 3: insert the generated dict into the reserved header
        // (runtime) step 4: at startup, load the dictionary into memory
        // (runtime) step 5: when request comes in, look up offset in dict and return the resource from mem.

        return 0;
    }

    // Two arguments = source and build dir, create resource files
    if (argc == 3)
    {
        path sourceDir {argv[1]};
        path buildDir {argv[2]};
        SETTINGS_FILE_DIR = buildDir / "settings";
        BINARY_FILE_DIR = buildDir / "data";

        cout << "source: " << sourceDir << ", build: " << buildDir << endl;

        vector<AssetImporter*> importers {
            new StbImageImporter {},
        };

        for (path file : directory_iterator{sourceDir})
        {
            for (AssetImporter* importer : importers)
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

namespace ResourceDatabase
{
    /* todo: types to support
     * todo: should be extendable by the game?
     * 1. shaders
     * 2. textures
     * 3. materials?
     * 4. meshes
     *
     * # workflow:
     * ## authoring time
     * - preprocess (assets are given additional import setting files)
     * ## compile time
     * - export (many asset formats are converted into a binary blob that the engine can load without processing)
     * ## runtime
     * - lookup (create an in-memory representation of an asset from its unique id)
     *
     * preprocessing needs to know what additional settings the runtime env wants
     * exporting needs to know how the runtime wants files layed out in memory
     *
     * need: memory layout for resource, deserialize from blob, serialize into blob
     */

    class ResourceLoader
    {
    public:
        std::unordered_set<const char*> getSupportedExtensions();
        void createDefaultImportSettings(nlohmann::json& settings);
        void serialize(std::ostream stream, const nlohmann::json& settings);
        void* deserialize(std::istream stream);
    };

    class ResourceDatabase
    {
        std::vector<ResourceLoader> m_resourceLoaders;

    public:
        void preprocess(const char* fileName)
        {
            const char* fileExtension;

            for (ResourceLoader loader : m_resourceLoaders)
            {
                if (loader.getSupportedExtensions().contains(fileExtension))
                {
                    nlohmann::json settings;
                    loader.createDefaultImportSettings(settings);
                    std::ofstream settingsFile;
                    settingsFile << settings;
                    break;
                }
            }
        }

        void compile(const char* folderName, const char* outputName)
        {
            std::vector<const char*> filesToCompile;
            std::ofstream outputStream;

            for (const char* fileName : filesToCompile)
            {
                const char* fileExtension;

                for (ResourceLoader loader : m_resourceLoaders)
                {
                    if (loader.getSupportedExtensions().contains(fileExtension))
                    {
                        std::ifstream settingsStream;
                        nlohmann::json settings;
                        settingsStream >> settings;
                        break;
                    }
                }
            }
        }

        void lookup(const char* packageName, u32 assetId);
    };
} // namespace ResourceDatabase
