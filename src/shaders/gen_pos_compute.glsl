#version 450 core

layout (local_size_x = 10,
	local_size_y = 10,
	local_size_z = 10) in;

layout (location = 0) uniform uint map_size;

layout (binding = 0, std430) readonly buffer map3D_in
{
    uint world_cells[];
};

layout (binding = 1, std430) writeonly buffer pos_buffer
{
    vec3 positions[];
};

layout (binding = 2) uniform atomic_uint current_index;

uint  getCellPos(uvec3 position)
{
    uint n_cells_per_row = uint(ceil(map_size / 32));
    return uint(floor(position.x / 32)	      +
		position.y * n_cells_per_row  +
		position.z * n_cells_per_row * map_size);
}

bool isAlive()
{
    return ((uint(world_cells[getCellPos(gl_GlobalInvocationID)])
	    & uint(uint(1) << uint(31 - mod(gl_GlobalInvocationID.x, 32)))) != uint(0));
    //return false;
}

void main(void)
{
    if (gl_GlobalInvocationID.x < map_size &&
	gl_GlobalInvocationID.y < map_size &&
	gl_GlobalInvocationID.z < map_size)
    {
	if (isAlive())
	{
	    positions[atomicCounterIncrement(current_index)] = vec3(gl_GlobalInvocationID); 
	    //positions[0] = vec3(1, 1, 1);
	}
    }
}
