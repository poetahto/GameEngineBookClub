#ifndef UNICODE
#define UNICODE
#endif

#include <iostream>
#include <windows.h>
// #include <GL/glew.h>
#include <windef.h>
#include <windowsx.h>
#include <hidusage.h>
#include <winuser.h>
#include "platform.h"

using namespace math;

// WINDOWS PLATFORM

// Input state saved from the windows message loop.
bool g_wantsToQuit { false };
bool g_upHeld { false };
bool g_downHeld { false };
bool g_leftHeld { false };
bool g_rightHeld { false };
bool g_hasMouseUpdate {};
Vec2 g_mousePos {};
Vec2 g_previousMousePos {};
Vec2 g_mouseDelta {};
Rect g_screenRect {};

// Low-level graphics device stuff for passing into OpenGL.
HDC g_deviceContext {};
HGLRC g_renderingContext {};

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

void platform::createWindow(const wchar_t *name, int width, int height)
{
    // Register the window class.
    const wchar_t* CLASS_NAME  = name;
    const wchar_t* WINDOW_NAME = name;

    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = GetModuleHandle(0);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        WINDOW_NAME,                    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,

        nullptr,       // Parent window
        nullptr,       // Menu
        nullptr,       // Instance handle
        nullptr        // Additional application data
        );

    ShowWindow(hwnd, SW_SHOWDEFAULT);
}

void platform::setupOpenGl()
{
    // Set up the pixel format info for rendering.
    PIXELFORMATDESCRIPTOR formatData;
    formatData.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    formatData.nVersion = 1;
    formatData.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    formatData.dwLayerMask = PFD_MAIN_PLANE;
    formatData.iPixelType = PFD_TYPE_COLORINDEX;
    formatData.cColorBits = 8;
    formatData.cDepthBits = 16;
    formatData.cAccumBits = 0;
    formatData.cStencilBits = 0;

    int formatInstance = ChoosePixelFormat(g_deviceContext, &formatData);

    if (!formatInstance)
    {
        printf("choose pixel format failed");
        PostQuitMessage(0);
    }

    // Try to apply the pixel format to our device.
    if (!SetPixelFormat(g_deviceContext, formatInstance, &formatData))
    {
        printf("set pixel format failed");
        PostQuitMessage(0);
    }

    // Create and activate a rendering context for OpenGL.
    g_renderingContext = wglCreateContext(g_deviceContext);
    wglMakeCurrent(g_deviceContext, g_renderingContext);
}

// Saves windows messages into a more useful state for polling during gameplay.
LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect {};
    RAWINPUTDEVICE device[1] {};
    UINT size {};
    RAWINPUT* rawInput {};
    RAWMOUSE rawMouse {};
    bool isVirtualDesktop {};
    int width {};
    int height {};
    bool result{};

    switch (message)
    {
    case WM_CREATE:
        g_deviceContext = GetDC(window);
        GetClientRect(window, &rect);
        g_screenRect =
            Rect{static_cast<float>(rect.right), static_cast<float>(rect.left),
                 static_cast<float>(rect.top), static_cast<float>(rect.bottom)};

        // Register a new raw input device for smooth mouse messages.
        device[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
        device[0].usUsage = HID_USAGE_GENERIC_MOUSE;
        device[0].dwFlags = RIDEV_INPUTSINK;
        device[0].hwndTarget = window;
        result = RegisterRawInputDevices(device, 1, sizeof(device[0]));

        //ShowCursor(false);

        return 0;

    case WM_INPUT:
        size = sizeof(RAWINPUT);
        static BYTE data[sizeof(RAWINPUT)];
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, data, &size, sizeof(RAWINPUTHEADER));
        rawInput = (RAWINPUT*)data;

        if (rawInput->header.dwType == RIM_TYPEMOUSE)
        {
            rawMouse = rawInput->data.mouse;

            //if ((rawMouse.usFlags & MOUSE_MOVE_ABSOLUTE) != 0)
            {
                isVirtualDesktop = (rawMouse.usFlags & MOUSE_VIRTUAL_DESKTOP) != 0;

                width = GetSystemMetrics(isVirtualDesktop ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);
                height = GetSystemMetrics(isVirtualDesktop ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);

                g_mouseDelta.x = static_cast<float>((rawMouse.lLastX / 65535.0f) * width);
                g_mouseDelta.y = static_cast<float>((rawMouse.lLastY / 65535.0f) * height);
                g_mousePos += g_mouseDelta;
                g_hasMouseUpdate = true;
            }
        }
        return 0;

    case WM_SIZE: // todo: find a way to let people register callbacks for this event, e.g. opengl resizing?
        GetClientRect(window, &rect);
        g_screenRect =
            Rect{static_cast<float>(rect.right), static_cast<float>(rect.left),
                 static_cast<float>(rect.top), static_cast<float>(rect.bottom)};
        return 0;

    // Handle the cases where the user wants to close the window.
    case WM_CLOSE:
    case WM_DESTROY:
        if (g_renderingContext)
        {
            wglDeleteContext(g_renderingContext);
        }

        if (g_deviceContext)
        {
            ReleaseDC(window, g_deviceContext);
        }

        g_wantsToQuit = true;
        return 0;

    // Handle the cases where the user presses some keys on their keyboard.
    case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
		bool isDown = (lParam & (1 << 31)) == 0;

        switch (wParam)
        {
        case 0x57: // W Keycode
        case VK_UP:
            g_upHeld = isDown;
            break;
        case 0x53: // S Keycode
        case VK_DOWN:
            g_downHeld = isDown;
            break;
        case 0x41: // A Keycode
        case VK_LEFT:
            g_leftHeld = isDown;
            break;
        case 0x44: // D Keycode
        case VK_RIGHT:
            g_rightHeld = isDown;
            break;
        }

        return 0;

    }

    // Return the defaults for any unhandled messages.
    return DefWindowProc(window, message, wParam, lParam);
}

void platform::updateInput()
{
    MSG message = {};
    g_hasMouseUpdate = false;

    // Use PeekMessage instead of the GetMessage because we don't want to block when no messages are available.
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        // These call into our custom WindowProc function.
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    // Now poll mouse data, so we can get accurate delta information even if no movements happened.
    if (!g_hasMouseUpdate)
    {
        g_mouseDelta.x = 0;
        g_mouseDelta.y = 0;
    }

    int x = static_cast<int>(g_screenRect.left + g_screenRect.width());
    int y = static_cast<int>(g_screenRect.top - g_screenRect.height());
    //SetCursorPos(x, y);

    //POINT point {};
    //GetCursorPos(&point);
    //g_mousePos.x = static_cast<float>(point.x);
    //g_mousePos.y = static_cast<float>(point.y);
    //g_mouseDelta = g_mousePos - g_previousMousePos;
    //g_previousMousePos = g_mousePos;
}

Rect platform::getWindowRect()
{
    return g_screenRect;
}

bool platform::wantsToQuit()
{
    return g_wantsToQuit;
}

void platform::swapWindowBuffers()
{
    SwapBuffers(g_deviceContext);
}

float getAxis(bool negative, bool positive)
{
    float result { 0 };

    if (positive) result++;
    if (negative) result--;

    return result;
}

Vec2 platform::getInputDirection()
{
    return Vec2 { getAxis(g_leftHeld, g_rightHeld), getAxis(g_downHeld, g_upHeld) };
}

Vec2 platform::getMouseDelta()
{
    return g_mouseDelta;
}
