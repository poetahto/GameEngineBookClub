#version 330 core

uniform mat4 World_From_Model;
in vec3 Model_Position;
in vec2 Model_UV;
out vec2 UV;

void main()
{
    gl_Position = World_From_Model * vec4(Model_Position, 1);
    UV = Model_UV;
}
