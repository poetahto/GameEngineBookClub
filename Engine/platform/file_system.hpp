#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include "types.hpp"
#include "enum_flags.hpp"

namespace FileSystem
{
    ENUM_FLAGS(FileAccess)
    enum class FileAccess
    {
        Read,
        Write
    };

    class File
    {
    public:
        template <typename T>
        T read();

        template <typename T>
        void read(void* destination, size_t count);

        void close();
        static File open(const char* fileName, FileAccess access = FileAccess::Read | FileAccess::Write);
    };

} // namespace FileSystem

#endif // FILE_SYSTEM_HPP
