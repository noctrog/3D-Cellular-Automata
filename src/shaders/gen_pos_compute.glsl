#version 450 core

layout (local_size_x = 10,
	local_size_y = 10,
	local_size_z = 10) in;

layout (location = 0) uniform uint map_size;
//layout (location = 1) uniform uint total_cells;

layout (binding = 0, std430) readonly buffer map3D_in
{
    uint world_cells[];
};

layout (binding = 1, std430) writeonly buffer pos_buffer
{
    uvec3 positions[];
};

layout (binding = 2) uniform atomic_uint current_index;

bool isAlive()
{
    return ((uint(world_cells[uint(floor((  (gl_GlobalInvocationID.x +
				     gl_GlobalInvocationID.y * map_size +
				     gl_GlobalInvocationID.z * pow(map_size, 2)) / 32)))])
	    & uint(uint(1) << uint(31 - mod(gl_GlobalInvocationID.x, 32)))) != uint(0));
}

void main(void)
{
    if (gl_GlobalInvocationID.x < map_size &&
	gl_GlobalInvocationID.y < map_size &&
	gl_GlobalInvocationID.z < map_size)
    {
	if (isAlive())
	{
	    positions[atomicCounterIncrement(current_index)] = gl_GlobalInvocationID; 
	}
    }
}
