#include <SDL2/SDL_events.h>

#include "sdl_imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <SDL2/SDL_video.h>

int processEvent(void*, SDL_Event* event)
{
    ImGui_ImplSDL2_ProcessEvent(event);
    return 0;
}

void ImGui::custom_init()
{
    // We need to assume a window + context has been set up before us.
    SDL_GLContext context = SDL_GL_GetCurrentContext();
    SDL_Window* window = SDL_GL_GetCurrentWindow();

    CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();

    // Handler to forward SDL Events onto Dear ImGui.
    SDL_AddEventWatch(processEvent, nullptr);
}

void ImGui::custom_free()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    DestroyContext();
}

void ImGui::custom_renderStart()
{
    ImGui_ImplSDL2_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    NewFrame();
}

void ImGui::custom_renderEnd()
{
    Render();
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}
