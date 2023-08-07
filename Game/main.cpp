#include "rendering/mesh.h"
#include "rendering/renderer.h"
#include "rendering/shader.h"
#include "SDL2/SDL.h"
#undef main

int main()
{
    SDL_Window* window;

    { // Initialization

        SDL_SetMainReady();
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,SDL_WINDOW_OPENGL);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_CreateContext(window);
        SDL_GL_SetSwapInterval(1); // enabling vsync

        // Set up the renderer
        renderer::initialize(800, 600);
    }

    Shader shader = Shader::fromFiles("test.vert", "test.frag");
    Mesh mesh = Mesh::quad();

    // game loop
    bool wantsToQuit{false};
    f32 time{0};

    while (!wantsToQuit)
    {
        // collect input
        SDL_Event sdlEvent;

        while (SDL_PollEvent(&sdlEvent) != 0)
        {
            if (sdlEvent.type == SDL_QUIT)
                wantsToQuit = true;
        }

        // render
        time += 1.0f / 144.0f; // todo: better delta time stuff
        renderer::clearScreen(0, 0, 0);
        shader.use();
        shader.setFloat("Time", time);
        mesh.draw();
        SDL_GL_SwapWindow(window);
    }

    // cleanup
    shader.free();
    mesh.free();
    SDL_Quit();
    return 0;
}
