#version 330 core

uniform vec3 color;
uniform sampler2D texture0;

in vec2 uv;

out vec4 finalColor;

void main()
{
    finalColor = texture(texture0, uv) * vec4(color, 1);
}
