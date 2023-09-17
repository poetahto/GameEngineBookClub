#include <GL/glew.h>
#include <imgui.h>
#include <SDL2/SDL.h>
#include <unordered_map>

#include "resource_manager.hpp"
#include "math/mat4.hpp"
#include "math/vec2.hpp"
#include "math/vec4.hpp"
#include "rendering/mesh.hpp"
#include "rendering/renderer.hpp"
#include "rendering/shader.hpp"
#include "rendering/game_texture.hpp"
#include "platform/application.hpp"
#include "platform/custom_imgui.hpp"
#include "string_name.hpp"
#undef main // lil bit of weirdness, thanks SDL. We don't want your custom entrypoint right now.

// todo: find better home for this logic, doesnt really need to be a class
class Terrain
{
public:
    explicit Terrain(GameTexture* heightmap, f32 heightScale)
    {
        s32 width = heightmap->texture->width;
        s32 height = heightmap->texture->height;

        std::vector<f32> vertices;

        // Generate all the vertices based on the heightmap image.
        for (s32 x = 0; x < width; x++)
        {
            for (s32 y = 0; y < height; y++)
            {
                u8* pixel = heightmap->texture->get<u8>(x, y);
                // Points
                vertices.push_back(static_cast<f32>(x));
                vertices.push_back(static_cast<f32>(*pixel) / 255.0f * heightScale);
                vertices.push_back(static_cast<f32>(y));
                // UVs
                vertices.push_back(static_cast<f32>(x));
                vertices.push_back(static_cast<f32>(y));
            }
        }

        // Generate all the indices for rendering the triangle strips
        std::vector<u32> indices{};

        for (s32 row = 0; row < height - 1; row++)
        {
            for (s32 column = 0; column < width; column++)
            {
                indices.push_back(row * width + column);
                indices.push_back((row + 1) * width + column);
            }

            indices.push_back(UINT_MAX);
        }

        auto vertexFormat = std::vector{3, 2};

        m_mesh = new Mesh{
            Renderer::VertexList::fromList(vertices),
            Renderer::VertexFormat::fromList(vertexFormat),
            Renderer::IndexList::fromList(indices)
        };
    }

    void free()
    {
        m_mesh->free();
    }

    void draw() const
    {
        m_mesh->draw(Renderer::TriangleStrips);
    }

private:
    Mesh* m_mesh;
};

int main()
{
    // === Initialization ===
    Application::init();
    Renderer::init();
    CustomImGui::init();
    Input::setResizeHandler(Renderer::resize);

    // === Game Loop ===

    // todo: stop loading everything directly from files on disk, need better asset pipeline
    ResourceManager resourceManager {"resources.pak"};
    Mesh mesh = Mesh::quad();
    Shader terrainShader = Shader::fromFiles("terrain.vert", "terrain.frag");
    GameTexture heightmap {resourceManager.load<Texture>(R"(source\heightmap.ppm)"_sn).data};
    f32 terrain_height = 25;
    GameTexture terrain_texture {resourceManager.load<Texture>("TerrainTest"_sn).data};
    Terrain terrain{&heightmap, terrain_height};

    bool wantsToQuit{false};
    f32 elapsedTime{};
    f32 deltaTime{};
    u64 frameStartTime{Application::getPerformanceCounter()};

    while (!wantsToQuit)
    {
        static Vec3 inputDirection{};
        static Vec3 inputRotation{};
        static f32 sensitivity{8};

        // gather input
        Input::pollInput();
        wantsToQuit = Input::wantsToQuit();

        static bool mouseShown{true};

        if (getKeyDown(Input::Escape))
        {
            SDL_SetRelativeMouseMode(mouseShown ? SDL_TRUE : SDL_FALSE);
            mouseShown = !mouseShown;
        }

        inputDirection.x = getAxis(Input::D, Input::A);
        inputDirection.y = getAxis(Input::Space, Input::LeftShift);
        inputDirection.z = getAxis(Input::W, Input::S);

        if (!mouseShown)
        {
            Vec2 mouseDelta = Input::getMouseDelta() * (1 / 144.0f) * sensitivity * Math::DEG2RAD;
            inputRotation.x += mouseDelta.x;
            inputRotation.y += mouseDelta.y;
        }

        CustomImGui::renderStart();

        // Player logic.
        Mat4 world_to_view{};
        {
            static Vec3 position{0, 0, -5};
            static Vec3 velocity{};
            static f32 speed{20};
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

            Vec3 targetVelocity = rotation_transform.
                transformDirection(inputDirection.normalized() * deltaTime * speed);
            velocity = Vec3::lerp(velocity, targetVelocity, acceleration * deltaTime);
            position += velocity;

            if (inputRotation.y > 90)
                inputRotation.y = 90;

            if (inputRotation.y < -90)
                inputRotation.y = -90;

            Mat4 view_to_world = rotation_transform * Mat4::translate(position);
            world_to_view = view_to_world.inverse();
        }

        // terrain logic
        static Vec3 terrain_bottomColor{1 / 255.0f, 21 / 255.0f, 5 / 255.0f};
        static Vec3 terrain_topColor{29 / 255.0f, 255 / 255.0f, 0 / 255.0f};
        static float terrain_uv_scale{0.025f};
        {
            ImGui::Begin("Terrain");
            ImGui::ColorEdit3("Terrain Bottom", &terrain_bottomColor.data);
            ImGui::ColorEdit3("Terrain Top", &terrain_topColor.data);
            ImGui::DragFloat("UV Scale", &terrain_uv_scale);
            ImGui::End();
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

            // shader.use();
            // shader.setVec2("uv_offset", offset);
            // shader.setMat4("model_to_world", Mat4::trs(position, rotation * Math::DEG2RAD, scale));
        }

        // Rendering Logic
        Mat4 view_to_clip;
        {
            // This should always happen before scene is rendered.
            static f32 view_fov{80};
            static Vec4 ortho_size{1, 1, 1, 1};
            static Vec3 backgroundColor{0, 0, 0};
            static bool wireframe_on{false};

            Vec2 size = Application::getSize();

            ImGui::Begin("Rendering");
            ImGui::ColorEdit3("Clear Color", &backgroundColor.data);
            ImGui::Text("Display: %fx%f [%ihz]", size.x, size.y, Application::getRefreshRate());
            ImGui::DragFloat("FOV", &view_fov, 0.1f);
            ImGui::DragFloat4("ortho size", &ortho_size.data, 0.01f);
            if (ImGui::Checkbox("Wireframe", &wireframe_on))
                glPolygonMode(GL_FRONT_AND_BACK, wireframe_on ? GL_LINE : GL_FILL);
            ImGui::End();

            view_to_clip = Mat4::perspective(0.1f, 1000, static_cast<s32>(size.x), static_cast<s32>(size.y), view_fov);

            Renderer::clearScreen(backgroundColor);
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
        // shader.use();
        // todo: these should be a uniform block because this stuff is pretty common?
        // shader.setFloat("time", elapsedTime);
        // shader.setMat4("world_to_view", world_to_view);
        // shader.setMat4("view_to_clip", view_to_clip);
        // mesh.draw(Renderer::Triangles);

        terrainShader.use();
        terrainShader.setFloat("uvScale", terrain_uv_scale);
        terrainShader.setTexture("texture0", terrain_texture);
        terrainShader.setFloat("height", terrain_height);
        terrainShader.setVec3("topColor", terrain_topColor);
        terrainShader.setVec3("bottomColor", terrain_bottomColor);
        terrainShader.setMat4("world_to_view", world_to_view);
        terrainShader.setMat4("view_to_clip", view_to_clip);
        terrain.draw();

        // This should always happen after scene is rendered.
        CustomImGui::renderEnd();
        Application::swapBuffers();

        // === TIME ===
        u64 frameEndTime = Application::getPerformanceCounter();
        deltaTime = static_cast<f32>(static_cast<f64>(frameEndTime - frameStartTime) / static_cast<f64>(
            Application::getPerformanceFrequency()));

        if (deltaTime >= 1) // clamp deltaTime if it goes too long
            deltaTime = 1;

        elapsedTime += deltaTime;
        frameStartTime = frameEndTime;
    }

    // === CLEANUP ===

    CustomImGui::free();
    // shader.free(); // todo: this shouldn't happen here.
    mesh.free();
    Application::free();
    return 0;
}
