#ifndef IMGUI_UTIL_H
#define IMGUI_UTIL_H

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>

class ImGuiWrapper
{
public:
    ImGuiWrapper(SDL_Window* window, SDL_GLContext context);
    void free();
    void processEvent(const SDL_Event& event);
    void renderStart();
    void renderEnd();
};

#endif // IMGUI_UTIL_H
