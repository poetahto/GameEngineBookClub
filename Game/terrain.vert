#version 330 core

uniform mat4 world_to_view;
uniform mat4 view_to_clip;
in vec3 vert_position_in_world;
out float y_position_in_world;

void main()
{
    gl_Position = vec4(vert_position_in_world, 1) * world_to_view * view_to_clip;
    y_position_in_world = vert_position_in_world.y;
}
