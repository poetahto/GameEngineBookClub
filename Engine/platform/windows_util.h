#ifndef WINDOWS_UTIL_H
#define WINDOWS_UTIL_H

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include "../types.h"

inline HWND createWindow(const wchar_t* name, u32 width, u32 height, WNDPROC winProc)
{
    // Register the window class.

    WNDCLASS wc = { };

    wc.lpfnWndProc   = winProc;
    wc.hInstance     = GetModuleHandle(nullptr);
    wc.lpszClassName = name;

    RegisterClass(&wc);

    // Create the window.

    HWND window = CreateWindowEx(
        0,                              // Optional window styles.
        name,                           // Window class
        name,                           // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,

        nullptr,       // Parent window
        nullptr,       // Menu
        nullptr,       // Instance handle
        nullptr        // Additional application data
        );

    ShowWindow(window, SW_SHOWDEFAULT);
    return window;
}

#endif // WINDOWS_UTIL_H
