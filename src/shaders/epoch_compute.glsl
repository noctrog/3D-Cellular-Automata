#version 450 core

layout (local_size_x = 10,
	local_size_y = 10,
	local_size_z = 10) in;

layout (location = 0) uniform uint map_size;
layout (location = 1) uniform uvec4 rule;

layout (binding = 0, std430) readonly buffer map3D_in
{
    uint world_cells[];       
};
layout (binding = 1, std430) buffer map3D_out
{
    uint aux_world_cells[];
};

// Add atomic counter
layout (binding = 2) uniform atomic_uint alive_cells;

uint num_neighbours()
{
    uint num_nb = 0;
    int i, j, k;
    for (i = -1; i < 2; ++i){
	for(j = -1; j < 2; ++j){
	    for (k = -1; k < 2; ++k){
		if (i != 0 && j != 0 && k != 0 &&
		    gl_GlobalInvocationID.x + i >= 0 && gl_GlobalInvocationID.x + i < map_size &&
		    gl_GlobalInvocationID.y + j >= 0 && gl_GlobalInvocationID.y + j < map_size &&
		    gl_GlobalInvocationID.z + k >= 0 && gl_GlobalInvocationID.z + k < map_size &&
		    bool(world_cells[uint((floor((gl_GlobalInvocationID.x + i) / 32) +
		    (gl_GlobalInvocationID.y + j) * map_size +
		    (gl_GlobalInvocationID.z + k) * pow(map_size, 2)))] & 
		    uint(((uint(1) << (uint(31) - uint(mod(gl_GlobalInvocationID.x + i, 32))))) != uint(0)))){
		    
		    ++num_nb;
		}
	    }
	}
    }

    return num_nb;
}

bool evolve()
{
    uint num_nb = num_neighbours();
    if (bool((world_cells[uint(floor((gl_GlobalInvocationID.x)/32.0f) +
	gl_GlobalInvocationID.y * map_size + gl_GlobalInvocationID.z * pow(map_size,2))] &
	(uint(1) << uint(31 - mod(gl_GlobalInvocationID.x, 32.0f))))))
    {
	if (num_nb <= rule.x || num_nb >= rule.y){
	    aux_world_cells[uint(floor((gl_GlobalInvocationID.x)/32.0f) + 
	    gl_GlobalInvocationID.y * map_size + gl_GlobalInvocationID.z * pow(map_size, 2))] &=
	    ~(uint(1) << uint(31 - mod(gl_GlobalInvocationID.x, 32.0f)));
	    return false;
	}
	else{
	    aux_world_cells[uint(floor((gl_GlobalInvocationID.x)/32.0f) + 
	    gl_GlobalInvocationID.y * map_size + gl_GlobalInvocationID.z * pow(map_size, 2))] |=
	    (uint(1) << uint(31 - mod(gl_GlobalInvocationID.x, 32.0f)));
	    return true;
	}
    }
    else
    {
	if (num_nb >= rule.z || num_nb <= rule.a){
	    aux_world_cells[uint(floor((gl_GlobalInvocationID.x)/32.0f) + 
	    gl_GlobalInvocationID.y * map_size + gl_GlobalInvocationID.z * pow(map_size, 2))] |=
	    (uint(1) << uint(31 - mod(gl_GlobalInvocationID.x, 32.0f)));
	    return true;
	}
	else{
	    aux_world_cells[uint(floor((gl_GlobalInvocationID.x)/32.0f) + 
	    gl_GlobalInvocationID.y * map_size + gl_GlobalInvocationID.z * pow(map_size, 2))] &=
	    ~(uint(1) << uint(31 - mod(gl_GlobalInvocationID.x, 32.0f)));
	    return false;
	}
    }
}


void main(void)
{
    // Only run if current compute unit is inside the world
    if (gl_GlobalInvocationID.x < map_size &&
	gl_GlobalInvocationID.y < map_size &&
	gl_GlobalInvocationID.z < map_size)
    {
	if (evolve())	atomicCounterIncrement(alive_cells);	
    }
}

