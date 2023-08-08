#version 330 core

out vec4 FragColor;
uniform float Time;

void main()
{
//    float t = (sin(Time) + 1) / 2;
    float t = 1;
    FragColor = vec4(0, t, 0, 1);
}
