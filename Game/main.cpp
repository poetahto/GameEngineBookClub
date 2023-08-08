#include "math/mat4.h"
#include "math/vec4.h"
#include "rendering/mesh.h"
#include "rendering/renderer.h"
#include "rendering/shader.h"
#include "SDL2/SDL.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#undef main

int main()
{
    SDL_Window* window;

    { // Initialization

        // sdl
        SDL_SetMainReady();
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,SDL_WINDOW_OPENGL);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GLContext context = SDL_GL_CreateContext(window);
        SDL_GL_SetSwapInterval(1); // enabling vsync

        // Dear ImGui
        ImGui::CreateContext();
        ImGui_ImplSDL2_InitForOpenGL(window, context);
        ImGui_ImplOpenGL3_Init();

        // Set up the renderer
        renderer::initialize(800, 600);
    }

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
            ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

            if (sdlEvent.type == SDL_QUIT)
                wantsToQuit = true;
        }

        // start ImGui
        ImGui_ImplSDL2_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // logic
        static Vec4 color;
        static Vec3 position;
        static f32 rotation;

        ImGui::Begin("Triangle State");
        ImGui::DragFloat3("Position", &position.data, 0.001f);
        ImGui::DragFloat("Rotation", &rotation);
        ImGui::ColorPicker4("Color", &color.data);
        ImGui::End();

        Mat4 worldFromModel = Mat4::rotateZ(rotation * math::DEG2RAD) * Mat4::translate(position);

        // render
        renderer::clearScreen(0, 0, 0);
        shader.use();
        shader.setFloat("Time", elapsedTime);
        shader.setVec4("Color", color);
        shader.setMat4("World_From_Model", worldFromModel);
        mesh.draw();

        // finish render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    shader.free();
    mesh.free();
    SDL_Quit();
    return 0;
}
