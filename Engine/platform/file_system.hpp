#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include <climits>
#include "enum_flags.hpp"

namespace FileSystem
{
    ENUM_FLAGS(FileAccess)
    enum class FileAccess
    {
        None = 0,
        All = INT_MAX,
        Read = 1 << 0,
        Write = 1 << 1,
    };

    class File
    {
    public:
        virtual ~File() = default;

        virtual void read(void* destination, size_t count) = 0;
        virtual void seek(size_t offset) = 0;
        virtual void close() = 0;
    };

    File* openFile(const char* fileName, FileAccess access = FileAccess::Read | FileAccess::Write);

} // namespace FileSystem

#endif // FILE_SYSTEM_HPP
