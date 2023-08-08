#version 330 core

uniform vec3 Color;
in vec2 UV;
out vec4 FragColor;

void main()
{
    FragColor = vec4(UV, 1, 1) * vec4(Color, 1);
}
