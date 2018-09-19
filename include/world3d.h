#pragma once

#include <world.h>
#include <string>
#include <vector>

class World3d : public World
{
    World3d(const std::string& r, size_t x, size_t y, size_t z);
    virtual ~World3d();

    bool	insert_cell_at(size_t x, size_t y, size_t z);
    bool	get_cell_at(size_t x, size_t y, size_t z);

    size_t	get_x_size();
    size_t	get_y_size();
    size_t	get_z_size();

    std::string	to_string();

    void	read_from_file(const std::string fn);
    void	read_from(std::ifstream& ifs);

    void	evolve();

    using CellRow = std::bitset
};
