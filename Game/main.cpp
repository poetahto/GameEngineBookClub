#include <imgui.h>

#include "math/mat4.h"
#include "math/vec4.h"
#include "rendering/mesh.h"
#include "rendering/renderer.h"
#include "rendering/shader.h"
#include "SDL2/SDL.h"
#include "imgui_wrapper"
#undef main

int main()
{
    SDL_Window* window;
    SDL_GLContext context;

    { // Initialization

        // sdl
        SDL_SetMainReady();
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,SDL_WINDOW_OPENGL);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        context = SDL_GL_CreateContext(window);
        SDL_GL_SetSwapInterval(1); // enabling vsync

        // Set up the renderer
        renderer::initialize(800, 600);
    }

    ImGuiWrapper imguiWrapper {window, context};
    Shader shader = Shader::fromFiles("test.vert", "test.frag");
    Mesh mesh = Mesh::quad();

    // game loop
    bool wantsToQuit{false};
    f32 elapsedTime {};
    f32 deltaTime {};
    u64 frameStartTime {SDL_GetPerformanceCounter()};

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
        static Vec3 color {1, 1, 1};
        static Vec3 position;
        static f32 rotation;

        ImGui::Begin("Triangle State");
        ImGui::DragFloat3("Position", &position.data, 0.001f);
        ImGui::DragFloat("Rotation", &rotation);
        ImGui::ColorEdit3("Color", &color.data);
        ImGui::End();

        Mat4 worldFromModel = Mat4::rotateZ(rotation * math::DEG2RAD) * Mat4::translate(position);

        // render
        renderer::clearScreen(0, 0, 0);
        shader.use();
        shader.setFloat("Time", elapsedTime);
        shader.setVec3("Color", color);
        shader.setMat4("World_From_Model", worldFromModel);
        mesh.draw();

        // finish render
        imguiWrapper.renderEnd();
        SDL_GL_SwapWindow(window);

        // update timing
        u64 frameEndTime = SDL_GetPerformanceCounter();
        deltaTime = static_cast<f32>(static_cast<f64>(frameEndTime - frameStartTime) / static_cast<f64>(SDL_GetPerformanceFrequency()));

        if (deltaTime >= 1) // clamp deltaTime if it goes too long
            deltaTime = 1;

        elapsedTime += deltaTime;
        frameStartTime = frameEndTime;
    }

    // cleanup
    imguiWrapper.free();
    shader.free();
    mesh.free();
    SDL_Quit();
    return 0;
}
