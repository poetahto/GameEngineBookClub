#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#undef main

#include "rendering\renderer.h"

int main()
{
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
                                          SDL_WINDOW_OPENGL);

    // Set up openGL integration
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1); // enabling vsync

    // Set up the renderer
    renderer::initialize(800, 600);
    renderer::clearScreen(0, 0, 0);

    // game loop
    for (;;)
    {
        SDL_PumpEvents();
        SDL_GL_SwapWindow(window);

        if (SDL_QuitRequested())
            break;
    }

    SDL_Quit();
    return 0;
}
