#include "rendering/mesh.h"
#include "rendering/renderer.h"
#include "rendering/shader.h"
#include "SDL2/SDL.h"
#undef main

int main()
{
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);

    // Set up openGL integration
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1); // enabling vsync

    // Set up the renderer
    renderer::initialize(800, 600);

    Shader shader = Shader::fromFiles("test.vert", "test.frag");
    Mesh mesh = Mesh::triangle();

    // game loop
    bool wantsToQuit {false};

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
        renderer::clearScreen(0, 0, 0);
        shader.use();
        mesh.draw();
        SDL_GL_SwapWindow(window);
    }

    // cleanup
    shader.free();
    mesh.free();
    SDL_Quit();
    return 0;
}
