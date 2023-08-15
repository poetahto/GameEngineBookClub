#include <SDL2/SDL_events.h>

#include "types.h"
#include "math/vec2.h"
#include "platform/application.h"

Input::ResizeHandler g_resizeHandler{};
bool g_wantsToQuit{};
u64 g_currentInputState{};
u64 g_prevInputState{};
Vec2 g_mouseDelta{};

void processKey(Input::KeyCode expected, SDL_Keycode actual, const SDL_Event& event)
{
    if (event.key.keysym.sym == actual && event.key.repeat == 0)
    {
        if (event.type == SDL_KEYDOWN)
            g_currentInputState = g_currentInputState | expected;

        if (event.type == SDL_KEYUP)
            g_currentInputState = g_currentInputState & ~expected;
    }
}

void Input::setResizeHandler(ResizeHandler resizeHandler)
{
    g_resizeHandler = resizeHandler;
}

void Input::pollInput()
{
    SDL_Event sdlEvent {};
    bool receivedMouseUpdate {};
    g_prevInputState = g_currentInputState;

    while (SDL_PollEvent(&sdlEvent) != 0)
    {
        if (sdlEvent.type == SDL_QUIT)
            g_wantsToQuit = true;

        processKey(W, SDLK_w, sdlEvent);
        processKey(A, SDLK_a, sdlEvent);
        processKey(S, SDLK_s, sdlEvent);
        processKey(D, SDLK_d, sdlEvent);
        processKey(Escape, SDLK_ESCAPE, sdlEvent);
        processKey(Space, SDLK_SPACE, sdlEvent);
        processKey(LeftShift, SDLK_LSHIFT, sdlEvent);

        if (sdlEvent.type == SDL_MOUSEMOTION)
        {
            g_mouseDelta.x = static_cast<f32>(sdlEvent.motion.xrel);
            g_mouseDelta.y = static_cast<f32>(-sdlEvent.motion.yrel);
            receivedMouseUpdate = true;
        }

        if (sdlEvent.type == SDL_WINDOWEVENT && sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED && g_resizeHandler != nullptr)
            g_resizeHandler(sdlEvent.window.data1, sdlEvent.window.data2);
    }

    if (!receivedMouseUpdate)
        g_mouseDelta = Vec2::ZERO;
}

f32 Input::getAxis(KeyCode positive, KeyCode negative)
{
    f32 result {0};

    if (getKey(positive))
        result += 1;

    if (getKey(negative))
        result -= 1;

    return result;
}

bool Input::getKey(KeyCode key)
{
    return (g_currentInputState & key) != 0;
}

bool Input::getKeyDown(KeyCode key)
{
    return (g_prevInputState & key) == 0 && (g_currentInputState & key) != 0;
}

bool Input::getKeyUp(KeyCode key)
{
    return (g_prevInputState & key) != 0 && (g_currentInputState & key) == 0;
}

bool Input::wantsToQuit()
{
    return g_wantsToQuit;
}

Vec2 Input::getMouseDelta()
{
    return g_mouseDelta;
}


