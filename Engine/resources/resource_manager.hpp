#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

// todo: support for async loading
// todo: singleton might make multithreading bad? we do really only want one instance of stuff loaded though. just go async if you want to avoid blocking

#include <unordered_map>

#include "binary_stream_builder.h"
#include "logger.hpp"
#include "types.hpp"
#include "string_name.hpp"
#include "texture.hpp"
#include "platform/file_system.hpp"

class ResourceManager;

/**
 * \brief A handle to some resource managed by a ResourceManager.
 * \tparam T The type of resource contained.
 */
template <typename T>
struct ResourceHandle
{
    StringName name;
    const T* data;
};

/**
 * \brief A system that keeps track of loaded resources and
 * serves them by hashed string ID's.
 */
class ResourceManager
{
public:
    ResourceManager(const char* packageFile) : m_packageFile{packageFile}
    {
        std::fstream packageFileStream {};
        packageFileStream.open(packageFile, std::ios::binary | std::ios::in);
        size_t assetCount {};

        BinaryStreamBuilder builder {&packageFileStream};
        builder.readFixed(&assetCount);

        for (size_t i {0}; i < assetCount; i++)
        {
            StringName::Hash hash {};
            size_t offset {};
            builder.readFixed(&hash);
            builder.readFixed(&offset);
            m_resourceOffsetLookup.emplace(hash, offset);
        }
    }

    template <typename T>
    ResourceHandle<T> load(StringName resourceName)
    {
        ResourceHandle<T> result{
            .name = resourceName,
        };

        if (m_loadedResourceTable.contains(resourceName))
        {
            // Someone else is already using this resource, so we can share it with them.
            result.data = static_cast<const T*>(m_loadedResourceTable[resourceName].data);
            m_loadedResourceTable[resourceName].referenceCount++;
        }
        else
        {
            // We are the first user of this resource, so construct it and cache it.
            size_t offset = m_resourceOffsetLookup[resourceName.hash];
            std::fstream packageFileStream {};
            packageFileStream.open(m_packageFile, std::ios::binary | std::ios::in);
            packageFileStream.seekg(offset);
            result.data = readResourceFrom<T>(packageFileStream);
            packageFileStream.close();

            LoadedResource resource;
            resource.data = result.data;
            resource.referenceCount = 1;

            m_loadedResourceTable.emplace(resourceName, resource);
        }

        return result;
    }

    template <typename T>
    void unload(StringName resourceName)
    {
        if (m_loadedResourceTable.contains(resourceName))
        {
            m_loadedResourceTable[resourceName].referenceCount--;

            if (m_loadedResourceTable[resourceName].referenceCount == 0)
            {
                // Nobody else is using this resource, so fully unload it.
                m_loadedResourceTable.erase(resourceName);
            }
        }
        else
        {
            Logger::log_error("Cannot unload a resource before it is loaded!");
        }
    }

private:
    struct LoadedResource
    {
        const void* data;
        u32 referenceCount;
    };

    const char* m_packageFile {};
    std::unordered_map<StringName::Hash, size_t> m_resourceOffsetLookup{};
    std::unordered_map<StringName, LoadedResource> m_loadedResourceTable{};
};

#endif // RESOURCE_MANAGER_HPP
