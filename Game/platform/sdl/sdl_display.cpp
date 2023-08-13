#include "config.h"
#ifdef PLATFORM_SDL

#include <SDL2/SDL.h>
#include "math/vec2.h"
#include "platform/application.h"

static SDL_Window* g_window{nullptr};

// Helper method for quickly reading errors from the SDL error buffer.
static void showSdlError()
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Error", SDL_GetError(), g_window);
}

void Application::init(const Settings& settings)
{
    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        showSdlError();

    u32 flags {};

#ifdef RENDERER_OPENGL
    flags |= SDL_WINDOW_OPENGL;

    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) != 0)
        showSdlError();

    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1) != 0)
        showSdlError();

    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) != 0)
        showSdlError();
#endif // RENDERER_OPENGL

    g_window = SDL_CreateWindow(settings.title, settings.x, settings.y, settings.width, settings.height, flags);

    if (g_window == nullptr)
        showSdlError();

#ifdef RENDERER_OPENGL
    SDL_GL_CreateContext(g_window);
#endif // RENDERER_OPENGL
}

void Application::free()
{
    SDL_DestroyWindow(g_window);
    SDL_Quit();
}

void Application::swapBuffers()
{
    SDL_GL_SwapWindow(g_window);
}

// Title Access
const char* Application::getTitle()
{
    return SDL_GetWindowTitle(g_window);
}

void Application::setTitle(const char* title)
{
    SDL_SetWindowTitle(g_window, title);
}

// todo: vec2int for this.
// Position Access
Vec2 Application::getPosition()
{
    s32 x, y;
    SDL_GetWindowPosition(g_window, &x, &y);
    return Vec2{static_cast<f32>(x), static_cast<f32>(y)};
}

void Application::setPosition(Vec2 position)
{
    s32 x = static_cast<s32>(position.x);
    s32 y = static_cast<s32>(position.y);
    SDL_SetWindowPosition(g_window, x, y);
}

// Size Access
Vec2 Application::getSize()
{
    s32 width, height;
    SDL_GetWindowSize(g_window, &width, &height);
    return Vec2{static_cast<f32>(width), static_cast<f32>(height)};
}

// todo: vec2int for this.
void Application::setSize(Vec2 size)
{
    s32 width = static_cast<s32>(size.x);
    s32 height = static_cast<s32>(size.y);
    SDL_SetWindowSize(g_window, width, height);
}

// Refresh Rate
s32 Application::getRefreshRate()
{
    SDL_DisplayMode mode;

    if (SDL_GetWindowDisplayMode(g_window, &mode) != 0)
        showSdlError();

    return mode.refresh_rate;
}

void Application::setRefreshRate(s32 rate)
{
    SDL_DisplayMode mode;

    if (SDL_GetWindowDisplayMode(g_window, &mode) != 0)
        showSdlError();

    mode.refresh_rate = rate;

    if (SDL_SetWindowDisplayMode(g_window, &mode) != 0)
        showSdlError();
}

#endif // PLATFORM_SDL
