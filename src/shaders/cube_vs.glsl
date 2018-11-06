#version 450 core 

layout (location = 0) uniform mat4 mvp;
layout (location = 1) uniform float map_size;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 center;

out VS_OUT
{
    vec4 color;
} vs_out;

void main(void)
{
    vec4 outColor;
    vec4 size = vec4(map_size, map_size, map_size, 1.0f);
    float b = max(0.0f, 1.0f - length(center - vec3(0, map_size, map_size))
			/sqrt(2)/map_size);
    float g = max(0.0f, 1.0f - length(center - vec3(map_size, map_size, 0))
			/sqrt(2)/map_size);
    float r = max(0.0f , 1.0f - length(center)/sqrt(2)/map_size);

    outColor = vec4 (r, g, b, 1.0f);
    gl_Position = mvp * vec4(position + center, 1.0f); 
    vs_out.color = outColor; 
}
