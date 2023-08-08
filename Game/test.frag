#version 330 core

uniform vec3 Color;
uniform sampler2D Texture;
uniform vec2 UV_Offset;
in vec2 UV;
out vec4 FragColor;

void main()
{
    FragColor = texture(Texture, UV + UV_Offset);
}
