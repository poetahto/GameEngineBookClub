#version 330 core

uniform vec3 bottomColor;
uniform vec3 topColor;
uniform float height;
uniform sampler2D texture0;
uniform float uvScale;
out vec4 finalColor;
in float y_position_in_world;
in vec2 uv;

vec3 lerp(vec3 a, vec3 b, float t)
{
    float x = (1 - t) * a.x + t * b.x;
    float y = (1 - t) * a.y + t * b.y;
    float z = (1 - t) * a.z + t * b.z;
    return vec3(x, y, z);
}

void main()
{
    float t = y_position_in_world / height;
    vec4 color = vec4(lerp(bottomColor, topColor, t), 1);
//    finalColor = vec4(uv, 0, 1);
    finalColor = texture(texture0, uv * uvScale) * color;
}
