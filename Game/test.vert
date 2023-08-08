#version 330 core

uniform mat4 model_to_world;
uniform vec2 uv_offset;

in vec3 vert_position_in_model;
in vec2 vert_uv;

out vec2 uv;

void main()
{
    gl_Position = vec4(vert_position_in_model, 1) * model_to_world;
    uv = vert_uv + uv_offset;
}
