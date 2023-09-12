#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "..\custom_imgui.hpp"

int processEvent(void*, SDL_Event* event)
{
    ImGui_ImplSDL2_ProcessEvent(event);
    return 0;
}

void CustomImGui::init()
{
    // We need to assume a window + context has been set up before us.
    SDL_GLContext context = SDL_GL_GetCurrentContext();
    SDL_Window* window = SDL_GL_GetCurrentWindow();

    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();

    // Handler to forward SDL Events onto Dear ImGui.
    SDL_AddEventWatch(processEvent, nullptr);
}

void CustomImGui::free()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
}

void CustomImGui::renderStart()
{
    ImGui_ImplSDL2_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void CustomImGui::renderEnd()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
