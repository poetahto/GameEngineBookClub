#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

// todo: support for async loading
// todo: singleton might make multithreading bad? we do really only want one instance of stuff loaded though. just go async if you want to avoid blocking

#include <unordered_map>
#include "binary_stream_builder.hpp"
#include "logger.hpp"
#include "types.hpp"
#include "string_name.hpp"

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

// A template that should be specialized by each new resource type, so we know how to write it into a package.
template <typename T>
void writeResourceTo(T* resource, BinaryStreamBuilder& packageFile)
{
    log_error(Logger::Channel::Resources, "Failed to find specialization for writing resource {}!", typeid(T).name());
};

// A template that should be specialized by each new resource type, so we know how to read it from a package.
template <typename T>
T* readResourceFrom(BinaryStreamBuilder& packageFile)
{
    log_error(Logger::Channel::Resources, "Failed to find specialization for reading resource {}!", typeid(T).name());
    return nullptr;
}

/**
 * \brief A system that keeps track of loaded resources and
 * serves them by hashed string ID's.
 */
class ResourceManager
{
public:
    explicit ResourceManager(const char* packageFile);

    template <typename T>
    ResourceHandle<T> load(StringName resourceName);

    void unload(StringName resourceName);

private:
    // Some resource that is currently loaded at runtime.
    struct LoadedResource
    {
        const void* data;
        u32 referenceCount;
    };

    const char* m_packageFile {};

    // Every asset's GUID from the package file, associated with offsets in the package.
    std::unordered_map<StringName::Hash, size_t> m_resourceOffsetLookup{};

    // Every resource that is currently being used at runtime.
    std::unordered_map<StringName, LoadedResource> m_loadedResourceTable{};
};

template <typename T>
ResourceHandle<T> ResourceManager::load(StringName resourceName)
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
        BinaryStreamBuilder builder {&packageFileStream};
        result.data = readResourceFrom<T>(builder);
        packageFileStream.close();

        LoadedResource resource;
        resource.data = result.data;
        resource.referenceCount = 1;

        m_loadedResourceTable.emplace(resourceName, resource);
    }

    return result;
}

#endif // RESOURCE_MANAGER_HPP
