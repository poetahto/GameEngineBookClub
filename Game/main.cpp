#include <imgui.h>
#include <unordered_map>

#include "math/mat4.h"
#include "math/vec4.h"
#include "rendering/mesh.h"
#include "rendering/renderer.h"
#include "rendering/shader.h"
#include "SDL2/SDL.h"
#include "imgui_wrapper.h"
#include "math/vec2.h"
#undef main

int main()
{
    // === Initialization ===
    s32 width = 800;
    s32 height = 600;

    // start SDL
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1); // enabling vsync

    // start OpenGL renderer
    renderer::initialize(width, height);

    // start ImGui
    ImGuiWrapper imguiWrapper{window, context};

    // === Game Loop ===

    Shader shader = Shader::fromMaterial("test.material");
    Mesh mesh = Mesh::quad();

    bool wantsToQuit{false};
    f32 elapsedTime{};
    f32 deltaTime{};
    u64 frameStartTime{SDL_GetPerformanceCounter()};

    while (!wantsToQuit)
    {
        static Vec3 inputDirection{};
        static Vec3 inputRotation{};
        static f32 sensitivity{8};

        // gather input
        {
            SDL_Event sdlEvent;

            while (SDL_PollEvent(&sdlEvent) != 0)
            // todo: i dont like this big switch, but idk if theres a better solution
            {
                imguiWrapper.processEvent(sdlEvent);

                if (sdlEvent.type == SDL_QUIT)
                    wantsToQuit = true;

                if ((sdlEvent.type == SDL_KEYUP || sdlEvent.type == SDL_KEYDOWN) && sdlEvent.key.repeat == 0)
                {
                    if (sdlEvent.key.keysym.sym == SDLK_w) inputDirection.z += sdlEvent.key.state == SDL_PRESSED
                                                                                   ? 1
                                                                                   : -1;
                    if (sdlEvent.key.keysym.sym == SDLK_s) inputDirection.z += sdlEvent.key.state == SDL_PRESSED
                                                                                   ? -1
                                                                                   : 1;
                    if (sdlEvent.key.keysym.sym == SDLK_d) inputDirection.x += sdlEvent.key.state == SDL_PRESSED
                                                                                   ? 1
                                                                                   : -1;
                    if (sdlEvent.key.keysym.sym == SDLK_a) inputDirection.x += sdlEvent.key.state == SDL_PRESSED
                                                                                   ? -1
                                                                                   : 1;
                    if (sdlEvent.key.keysym.sym == SDLK_SPACE) inputDirection.y += sdlEvent.key.state == SDL_PRESSED
                        ? 1
                        : -1;
                    if (sdlEvent.key.keysym.sym == SDLK_LSHIFT) inputDirection.y += sdlEvent.key.state == SDL_PRESSED
                        ? -1
                        : 1;
                }

                static bool mouseShown = true;

                if (sdlEvent.type == SDL_KEYDOWN && sdlEvent.key.repeat == 0 && sdlEvent.key.keysym.sym == SDLK_ESCAPE)
                {
                    SDL_SetRelativeMouseMode(mouseShown ? SDL_TRUE : SDL_FALSE);
                    mouseShown = !mouseShown;
                }

                if (sdlEvent.type == SDL_MOUSEMOTION && !mouseShown)
                {
                    inputRotation.x += static_cast<f32>(sdlEvent.motion.xrel) * deltaTime * sensitivity * math::DEG2RAD;
                    inputRotation.y -= static_cast<f32>(sdlEvent.motion.yrel) * deltaTime * sensitivity * math::DEG2RAD;
                }

                if (sdlEvent.type == SDL_WINDOWEVENT && sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED)
                    renderer::resize(sdlEvent.window.data1, sdlEvent.window.data2);
            }
        }

        imguiWrapper.renderStart();

        // Player logic.
        Mat4 world_to_view{};
        {
            static Vec3 position{0, 0, -5};
            static Vec3 velocity{};
            static f32 speed{1};
            static f32 acceleration{10};
            ImGui::Begin("Player");
            ImGui::Text("Input: %f %f", inputDirection.x, inputDirection.z);
            ImGui::Text("Position: %f %f %f", position.x, position.y, position.z);
            ImGui::Text("Rotation: %f %f", inputRotation.x, inputRotation.y);
            ImGui::DragFloat("speed", &speed, 0.1f);
            ImGui::DragFloat("acceleration", &acceleration, 0.1f);
            ImGui::DragFloat("sensitivity", &sensitivity);
            ImGui::End();

            Mat4 rotation_transform = Mat4::rotate(inputRotation.y, inputRotation.x, 0);

            Vec3 targetVelocity = rotation_transform.transformDirection(inputDirection.normalized() * deltaTime * speed);
            velocity = Vec3::lerp(velocity, targetVelocity, acceleration * deltaTime);
            position += velocity;

            if (inputRotation.y > 90)
                inputRotation.y = 90;

            if (inputRotation.y < -90)
                inputRotation.y = -90;

            Mat4 view_to_world = rotation_transform * Mat4::translate(position);
            world_to_view = view_to_world.inverse();
        }

        // Quad logic.
        {
            static Vec3 position;
            static Vec3 scale{Vec3::ONE};
            static Vec3 rotation;
            static Vec2 offset{};

            ImGui::Begin("Quad State");

            ImGui::DragFloat3("Position", &position.data, 0.001f);
            ImGui::DragFloat3("Rotation", &rotation.data);
            ImGui::DragFloat3("Scale", &scale.data, 0.001f);
            ImGui::DragFloat2("UV Offset", &offset.data, 0.001f);
            ImGui::End();

            shader.use();
            shader.setVec2("uv_offset", offset);
            shader.setMat4("model_to_world", Mat4::trs(position, rotation * math::DEG2RAD, scale));
        }

        // Rendering Logic
        Mat4 view_to_clip;
        {
            SDL_DisplayMode display;
            SDL_GetWindowDisplayMode(window, &display);

            // This should always happen before scene is rendered.
            static f32 view_fov{80};
            static Vec4 ortho_size{1, 1, 1, 1};
            static Vec3 backgroundColor{0, 0, 0};

            ImGui::Begin("Rendering");
            ImGui::ColorEdit3("Clear Color", &backgroundColor.data);
            ImGui::Text("Display: %ix%i [%ihz]", display.w, display.h, display.refresh_rate);
            ImGui::Text("Format: %s", SDL_GetPixelFormatName(display.format));
            ImGui::DragFloat("FOV", &view_fov, 0.1f);
            ImGui::DragFloat4("ortho size", &ortho_size.data, 0.01f);
            ImGui::End();

            s32 cur_width, cur_height;
            SDL_GetWindowSize(window, &cur_width, &cur_height);
            view_to_clip = Mat4::perspective(0.1f, 10, cur_width, cur_height, view_fov);

            renderer::clearScreen(backgroundColor);
        }

        // Timing Window
        {
            constexpr int historySize = 120;
            static f32 remainingTime{0};
            static f32 view_fps{};
            static f32 view_fps_history[historySize]{};

            if (remainingTime <= 0)
            {
                remainingTime = 0.1f;

                if (deltaTime > 0)
                    view_fps = 1.0f / deltaTime;

                for (s32 i = historySize - 2; i >= 0; i--)
                    view_fps_history[i + 1] = view_fps_history[i];

                view_fps_history[0] = view_fps;
            }
            else remainingTime -= deltaTime;

            ImGui::Begin("Time");
            ImGui::Text("FPS: %f", view_fps);
            ImGui::PlotLines("##fps graph", view_fps_history, historySize, 0, nullptr, 0, 500, ImVec2(0, 100));
            ImGui::Text("Elapsed Time: %f", elapsedTime);
            ImGui::End();
        }

        // Probably submit this to a rendering queue so it can be batched?
        shader.use();
        // todo: these should be a uniform block because this stuff is pretty common?
        shader.setFloat("time", elapsedTime);
        shader.setMat4("world_to_view", world_to_view);
        shader.setMat4("view_to_clip", view_to_clip);
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
