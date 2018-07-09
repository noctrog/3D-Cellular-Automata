#version 450 core 

layout (location = 0) uniform mat4 mvp;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 center;

out VS_OUT
{
    vec4 color;
} vs_out;

void main(void)
{
    vec4 outColor;
    if (center.x == 0.0f) outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    else		  outColor = vec4 (0.3f, 0.3f, 0.9f, 1.0f);
    gl_Position = mvp * vec4(position + center, 1.0f); 
    vs_out.color = outColor; 
}
