#include <imgui.h>

#include "math/mat4.h"
#include "math/vec4.h"
#include "rendering/mesh.h"
#include "rendering/renderer.h"
#include "rendering/shader.h"
#include "SDL2/SDL.h"
#include "imgui_wrapper.h"
#undef main

int main()
{
    // === Initialization ===

    // start SDL
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1); // enabling vsync

    // start OpenGL renderer
    renderer::initialize(800, 600);

    // start ImGui
    ImGuiWrapper imguiWrapper{window, context};

    // === Game Loop ===

    Shader shader = Shader::fromFiles("test.vert", "test.frag");
    Mesh mesh = Mesh::quad();
    bool wantsToQuit{false};
    f32 elapsedTime{};
    f32 deltaTime{};
    u64 frameStartTime{SDL_GetPerformanceCounter()};

    while (!wantsToQuit)
    {
        // collect input
        SDL_Event sdlEvent;

        while (SDL_PollEvent(&sdlEvent) != 0)
        {
            imguiWrapper.processEvent(sdlEvent);

            if (sdlEvent.type == SDL_QUIT)
                wantsToQuit = true;
        }

        imguiWrapper.renderStart();

        // logic
        static Vec3 color{1, 1, 1};
        static Vec3 position;
        static f32 rotation;

        ImGui::Begin("Triangle State");
        ImGui::DragFloat3("Position", &position.data, 0.001f);
        ImGui::DragFloat("Rotation", &rotation);
        ImGui::ColorEdit3("Color", &color.data);
        ImGui::End();

        Mat4 worldFromModel = Mat4::rotateZ(rotation * math::DEG2RAD) * Mat4::translate(position);

        // === RENDER ===

        {
            // This should always happen before scene is rendered.
            ImGui::Begin("Rendering");
            static Vec3 backgroundColor{0, 0, 0};
            ImGui::ColorEdit3("Clear Color", &backgroundColor.data);
            ImGui::End();

            renderer::clearScreen(backgroundColor);
        }
        {
            // Timing Window
            ImGui::Begin("Time");
            ImGui::Text("FPS: %f", 1.0f / deltaTime);
            ImGui::Text("Delta Time: %f", deltaTime);
            ImGui::Text("Elapsed Time: %f", elapsedTime);
            ImGui::Text("Ticks: %llu", frameStartTime);
            ImGui::Text("Frequency: %llu", SDL_GetPerformanceFrequency());
            ImGui::End();
        }

        // This should change based on scene contents, and materials
        shader.use();
        shader.setFloat("Time", elapsedTime);
        shader.setVec3("Color", color);
        shader.setMat4("World_From_Model", worldFromModel);
        mesh.draw();

        // This should always happen after scene is rendered.
        imguiWrapper.renderEnd();
        SDL_GL_SwapWindow(window);

        // === TIME ===

        u64 frameEndTime = SDL_GetPerformanceCounter();
        deltaTime = static_cast<f32>(static_cast<f64>(frameEndTime - frameStartTime) / static_cast<f64>(
            SDL_GetPerformanceFrequency()));

        if (deltaTime >= 1) // clamp deltaTime if it goes too long
            deltaTime = 1;

        elapsedTime += deltaTime;
        frameStartTime = frameEndTime;
    }

    // === CLEANUP ===

    imguiWrapper.free();
    shader.free();
    mesh.free();
    SDL_Quit();
    return 0;
}
