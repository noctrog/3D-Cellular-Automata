#pragma once

#include <string>
#include <vector>
#include <memory>
#include <world.h>
#include <Rule.h>

class World3d : public World
{
public:
    World3d(const std::string& r, size_t _size);
    World3d(const std::string& fn);
    virtual ~World3d();

    bool	    insert_cell_at(size_t x, size_t y, size_t z);
    bool	    get_cell_at(size_t x, size_t y, size_t z);
    size_t	    get_num_nb(int x, int y, int z);

    size_t	    get_size();

    std::string	    to_string();

    void	    read_from_file(const std::string& fn);
    virtual void    read_from(std::ifstream& ifs) override;

    virtual void    evolve() override;

    float*	    get_positions_buffer();
    size_t	    get_positions_buffer_size();

    void	    reset_world();

private:
    using CellRow   = std::vector<bool>;
    using CellGrid  = std::vector<CellRow>;
    using CellCube  = std::vector<CellGrid>;

    CellCube world;
    CellCube aux_world;

    size_t size;
    Rule the_rule;
    
    using vector3 = std::array<float, 3>;
    std::vector<vector3> positions_buffer;
};
