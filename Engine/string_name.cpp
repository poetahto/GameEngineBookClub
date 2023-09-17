#include "string_name.hpp"

StringName operator ""_sn(const char* rawName, size_t size)
{
    StringName result {StringName::createHash(rawName)};

#if _DEBUG
    result.name = rawName;
#endif // _DEBUG

    return result;
}
