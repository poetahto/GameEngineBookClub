#include "resource_manager.hpp"
#include "platform/file_system.hpp"

using namespace FileSystem;

ResourceManager::ResourceManager(const char* packageFile): m_packageFile{packageFile}
{
    /*
     * Package file binary layout:
     * 1) the number for total assets in the package
     * 2) key-value pairs for each asset: key = name hash, value = offset from start
     * 3) the asset data, looked up by offsets in header
     */

    size_t assetCount {};
    BinaryStreamBuilder builder {packageFile};
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

void ResourceManager::unload(StringName resourceName)
{
    Logger::DefaultLogChannelJanitor janitor {Logger::Channel::Resources};

    if (m_loadedResourceTable.contains(resourceName))
    {
        LoadedResource& resource = m_loadedResourceTable[resourceName];
        resource.referenceCount--;

        if (resource.referenceCount == 0)
        {
            // Nobody else is using this resource, so fully unload it.
            // todo: change the deallocation scheme when we change the alloc scheme in readResourceFrom(...)
            free(const_cast<void*>(resource.data));
            m_loadedResourceTable.erase(resourceName);
            Logger::log_info("Ref count for {} reached 0, so it was unloaded", resourceName.hash);
        }
        else
        {
            Logger::log_info("Decreased ref count on {} to {}", resourceName.hash, resource.referenceCount);
        }
    }
    else
    {
        Logger::log_error("Cannot unload a resource before it is loaded!");
    }
}
