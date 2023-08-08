#version 330 core

in vec3 Model_Position;
uniform mat4 World_From_Model;

void main()
{
    gl_Position = World_From_Model * vec4(Model_Position, 1);
}
