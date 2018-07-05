#version core 450

uniform mat4 mvp;

layout (binding = 0) in vec3 position;
layout (binding = 1) in vec3 center;

out VS_OUT
{
    color;
} vs_out;

void main(void)
{
    gl_Position = mvp * vec4(position + center);
    vs_out.color = vec4 (0.3f, 0.7f, 0.1f, 1.0f);
}
