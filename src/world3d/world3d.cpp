#include <world3d.h>

World3d::World3d(const std::string& r, size_t _size)
    : World(), size(_size), the_rule(r)
{
    reset_world();
}

World3d::World3d(const std::string& fn)
{
    read_from_file(fn);
}

World3d::~World3d()
{

}

void World3d::reset_world()
{
    world.resize(size, false);
    for (auto y : world){
	y.resize(size, false);
	for(auto x : y){
	    x.resize(size, false);
	}
    }

    aux_world.resize(size, false);
    for (auto y : aux_world){
	y.resize(size, false);
	for(auto x : y){
	    x.resize(size, false);
	}
    }
}

bool World3d::insert_cell_at(size_t x, size_t y, size_t z)
{
    if (x < size && y < size && z < size){
	aux_world[k][j][i] = true;
	positions_buffer.push_back({{static_cast<float>(x),
				     static_cast<float>(y),
				     static_cast<float>(z)}});
	num_beings++;
	return true;
    }
    else {
	return false;
    }
}

bool World3d::get_cell_at(size_t x, size_t y, size_t z)
{
    if (x < size && y < size && z < size){
	return world[z][y][x];
    }
}

size_t World3d::get_size()
{
    return size;
}

std::string World3d::to_string()
{
    std::ostringstream ss;

    for (auto z : world){
	for (auto y : z){
	    for (auto x : y){
		ss << (x) ? '1' : '0';
	    }
	    ss << std::endl;
	}
	ss << std::endl;
    }

    return ss.str();
}

void World3d::read_from_file(const std::string& fn)
{
    std::ifstream ifs(fn);
    read_from(ifs);
}

void World3d::read_from(std::ifstream& ifs)
{
    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    // load rule
    std::string rule_string;
    std::getline(ifs, rule_string);
    rule_string = rule_string.sub(6, 4);
    the_rule.set_rule(rule_string);   

    // get world size
    std::string size_string;
    std::getline(ifs, size_string);
    size = std::stoi(size_string.substr(6, std::string::npos));

    // Initialize world
    reset_world();

    // reset positions buffer
    positions_buffer.clear();

    // read cells
    std::string current_line;
    std::getline(ifs, current_line);
    if (current_line != "cells:"){
	throw std::runtime_error("Bad file format");
    }
    else {
	while (std::getline(ifs, current_line))
	{
	    size_t x, y, z;
	    std::stringstream ss(current_line);
	    if (!(ss >> x >> y >> z))		      break;
	    if (!(x < size && y < size && z < size))  break;

	    world[z][y][x] = true;
	    positions_buffer.push_back({{static_cast<float>(x),
					static_cast<float>(y),
					static_cast<float>(z)}});
	    nbeings++;
	}
    }
}

void World3d::evolve()
{
    num_beings = 0;

    for (int i = 0; i < size; ++i){
	for (int j = 0; j < size; ++j){
	    for (int k = 0; k < size; ++k){
		uint8_t num_nb = get_num_nb();
		if (get_cell_at[k][j][i]){
		    if (the_rule.survives()){
			insert_cell_at(k, j, i);
		    }
		    else {
			aux_world[k][j][i] = false;
		    }
		}
		else{
		    if (the_rule.is_born(num_nb)){
			insert_cell_at(k, j, i);
		    }
		    else {
			aux_world[k][j][i] = false;
		    }
		}
	    }
	}
    }

    world.swap(aux_world);
}

float* World3d::get_positions_buffer()
{
    return positions_buffer.data();
}

size_t World3d::get_positions_buffer_size()
{
    return (3 * sizeof(float) * positions_buffer.size());
}
