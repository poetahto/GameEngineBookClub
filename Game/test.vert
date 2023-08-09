#version 330 core

uniform mat4 model_to_world;
uniform mat4 world_to_view;
uniform mat4 view_to_clip;
uniform vec2 uv_offset;

in vec3 vert_position_in_model;
in vec2 vert_uv;

out vec2 uv;

void main()
{
    gl_Position = vec4(vert_position_in_model, 1) * model_to_world * world_to_view * view_to_clip;
    uv = vert_uv + uv_offset;
}
