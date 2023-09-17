#pragma once

#include <Windows.h>
#include <string>

inline std::wstring utf8ToUtf16(const std::string& utf8)
{
    std::wstring utf16 {};

    if (utf8.empty())
        return utf16;

    constexpr DWORD flags = MB_ERR_INVALID_CHARS;
    const int utf8Length = static_cast<int>(utf8.length());

    const int utf16Length = MultiByteToWideChar(CP_UTF8, flags, utf8.data(), utf8Length, nullptr, 0);
    utf16.resize(utf16Length);
    MultiByteToWideChar(CP_UTF8, flags, utf8.data(), utf8Length, &utf16[0], utf16Length);
    return utf16;
}

inline std::string getLastErrorAsString()
{
    DWORD errorId {GetLastError()};

    if (errorId == 0)
        return std::string{};

    LPSTR messageBuffer {};

    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, errorId, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &messageBuffer, 0, nullptr);

    std::string message {messageBuffer, size};
    LocalFree(messageBuffer);
    return message;
}
