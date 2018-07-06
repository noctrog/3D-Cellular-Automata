#version 450 core 

layout (location = 1) uniform mat4 mvp;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 center;

out VS_OUT
{
    vec4 color;
} vs_out;

void main(void)
{
    const vec4 vertices[3] = { vec4( 0.25, -0.25, 0.5, 1.0),
			       vec4(-0.25, -0.25, 0.5, 1.0),
			       vec4( 0.25,  0.25, 0.5, 1.0)};
    gl_Position = vertices[gl_VertexID] + vec4(position, 1.0f)/* + vec4(center, 1.0f) */;
    //gl_Position = mvp * vec4(position + center, 1.0f);
    vs_out.color = vec4 (0.3f, 0.7f, 0.1f, 1.0f);
}
