#version 450 core 

in VS_OUT
{
    vec4 color;
} fs_in;

out vec4 color;

void main(void)
{
    color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}
