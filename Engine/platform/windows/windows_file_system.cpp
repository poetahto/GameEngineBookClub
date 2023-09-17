#ifdef _WIN32

#include "logger.hpp"
#include "windows_util.hpp"
#include "platform/file_system.hpp"

using namespace FileSystem;

class WindowsFile final : public File
{
public:
    explicit WindowsFile(const char* fileName, HANDLE fileHandle) : m_fileHandle{fileHandle}, m_fileName{fileName}
    {
    }

    void read(void* destination, size_t count) override
    {
        if (ReadFile(m_fileHandle, destination, static_cast<int>(count), nullptr, nullptr) != TRUE)
            Logger::log_error("WIN32: Failed to read {} bytes from {}. Reason: {}", count, m_fileName, getLastErrorAsString());
    }

    void seek(size_t offset) override
    {
        DWORD result = SetFilePointer(m_fileHandle, static_cast<int>(offset), nullptr, FILE_BEGIN);

        if (result == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
            Logger::log_error("WIN32: Failed to seek offset {}. Reason: {}", offset, getLastErrorAsString());
    }

    void close() override
    {
        if (CloseHandle(m_fileHandle) != TRUE)
            Logger::log_error("WIN32: Failed to close file {}. Reason: {}", m_fileName, getLastErrorAsString());
    }

private:
    HANDLE m_fileHandle {};
    const char* m_fileName {};
};

File* FileSystem::openFile(const char* fileName, FileAccess access)
{
    DWORD desiredAccess = 0;

    if ((access & FileAccess::Read) != FileAccess::None)
        desiredAccess |= GENERIC_READ;

    if ((access & FileAccess::Write) != FileAccess::None)
        desiredAccess |= GENERIC_WRITE;

    HANDLE fileHandle = CreateFileA(
        fileName,
        desiredAccess,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (fileHandle == INVALID_HANDLE_VALUE)
        Logger::log_error("WIN32: Failed to open {}. Reason: {}", fileName, getLastErrorAsString());

    return new WindowsFile{fileName, fileHandle};
}

#endif // _WIN32
