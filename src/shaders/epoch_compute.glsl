#version 450 core

layout (local_size_x = 10,
	local_size_y = 10,
	local_size_z = 10) in;

layout (location = 0) uniform uint map_size;
layout (location = 1) uniform uvec4 rule;

layout (binding = 0, std430) readonly buffer map3D_in
{
    uint in_world_cells[];       
};
    
layout (binding = 1, std430) buffer map3D_out
{
    uint out_world_cells[];
};

// Add atomic counter
layout (binding = 2) uniform atomic_uint alive_cells;

uint  getCellPos(uvec3 position)
{
    uint n_cells_per_row = uint(ceil(map_size / 32));
    return uint(floor(position.x / 32)	      +
		position.y * n_cells_per_row  +
		map_size * position.z * n_cells_per_row);
}

bool  getCell(uvec3 position)
{
    return bool(
	in_world_cells[getCellPos(position)] & int(uint(1) << (uint(31) - uint(mod(position.x, 32))))
    );
}


uint numNeighbours()
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
		    getCell(uvec3(gl_GlobalInvocationID.x + i, gl_GlobalInvocationID.y + j, gl_GlobalInvocationID.z + k))){
		    
		    ++num_nb;
		}
	    }
	}
    }

    return num_nb;
}

void evolve()
{
    uint num_nb = numNeighbours();
    if (getCell(gl_GlobalInvocationID))
    {
	if (num_nb <= rule.x || num_nb >= rule.y){
	    out_world_cells[getCellPos(gl_GlobalInvocationID)] &=
				~(uint(1) << uint(31 - mod(gl_GlobalInvocationID.x, 32.0f)));
	}
	else{
	    out_world_cells[getCellPos(gl_GlobalInvocationID)] |=
				(uint(1) << uint(31 - mod(gl_GlobalInvocationID.x, 32.0f)));
	    atomicCounterIncrement(alive_cells);
	}
    }
    else
    {
	if (num_nb >= rule.z || num_nb <= rule.a){
	    out_world_cells[getCellPos(gl_GlobalInvocationID)] |=
				(uint(1) << uint(31 - mod(gl_GlobalInvocationID.x, 32.0f)));
	    atomicCounterIncrement(alive_cells);
	}
	else{
	    out_world_cells[getCellPos(gl_GlobalInvocationID)] &=
				~(uint(1) << uint(31 - mod(gl_GlobalInvocationID.x, 32.0f)));
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
	evolve();	
    }
}

