#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

// todo: support for async loading
// todo: singleton might make multithreading bad? we do really only want one instance of stuff loaded though. just go async if you want to avoid blocking

#include "logger.hpp"
#include "types.hpp"
#include "string_name.hpp"
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
 * \brief An abstract template that can be specialized to define
 * strategies for resource creation and deletion.
 * \tparam T The type of resource that is handled.
 */
template <typename T>
class ResourceFactory
{
    T* load(void* binaryData)
    {
        Logger::log_error("Failed to find resource specialization!");
        return nullptr;
    }

    void unload(T* resource)
    {
        Logger::log_error("Failed to find resource specialization!");
    }

    static ResourceFactory getInstance()
    {
        Logger::log_error("Failed to find resource specialization!");
        return nullptr;
    }
};

/**
 * \brief A system that keeps track of loaded resources and
 * serves them by hashed string ID's.
 */
class ResourceManager
{
public:
    template <typename T>
    ResourceHandle<T> load(StringName resourceName)
    {
        ResourceHandle<T> result{
            .name = resourceName,
        };

        if (m_loadedResourceTable.contains(resourceName))
        {
            // Someone else is already using this resource, so we can share it with them.
            result.data = static_cast<T*>(m_loadedResourceTable[resourceName].data);
            m_loadedResourceTable[resourceName].referenceCount++;
        }
        else
        {
            // We are the first user of this resource, so construct it and cache it.
            ResourceFactory<T> factory = ResourceFactory<T>::getInstance();
            T* resource = factory.load();
            result.data = resource;
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
                ResourceFactory<T> factory = ResourceFactory<T>::getInstance();
                factory.unload(static_cast<T*>(m_loadedResourceTable[resourceName].data));
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
        void* data;
        u32 referenceCount;
    };

    std::unordered_map<StringName, LoadedResource> m_loadedResourceTable{};
};

#endif // RESOURCE_MANAGER_HPP
