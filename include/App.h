#pragma once

#include <memory>
#include <string>
#include <fstream>

#include <GL/gl3w.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <Shader.h>
#include <Rule.h>

namespace sdl2
{
    struct SDL_Deleter
    {
	void operator()(SDL_Window* ptr) { if (ptr) SDL_DestroyWindow(ptr); }
	void operator()(SDL_GLContext* ptr) { if (ptr) SDL_GL_DeleteContext(ptr); }
    };

    using WindowPtr = std::unique_ptr<SDL_Window, SDL_Deleter>;
    using GLContextPtr = std::unique_ptr<SDL_GLContext, SDL_Deleter>;
}

// Each World Unit will contain 8 cells
// The value stored is if they are either 
// alive (true, 1) or dead (false, 0);
struct world_unit
{
    uint8_t cells;
};

// To store the next number of alive cells
struct uint_ssbo
{
    uint32_t cell_count;
};

class App
{
public:
    App ();
    App (const std::string& _mapFilePath);
    virtual ~App ();

    /// Runs at the beginning, set up SDL and OpenGL
    virtual void  setup();

    /// runs once right after setup
    virtual void  startup();

    /// The program that runs each frame
    virtual void  render();

    /// Executes everything in order: setup, startup, render loop, shutdown 
    virtual void  run();

    /// Runs after render loop
    virtual void  shutdown();

    /// Closes SDL and OpenGL, frees memory
    virtual void  terminate();

private:
    void	  SDLinit();
    void	  GLinit();

    void	  UpdateTime();

    sdl2::WindowPtr	window;
    sdl2::GLContextPtr	glcontext;

    /// Saves the current value of time, in seconds
    float currentTime;

    /* ---   User input   --- */
    bool	    b_run_single_epoch;
    bool	    b_auto_epoch;
    float	    auto_epoch_rate;

    /* ---  File objects  --- */
    //std::fstream mapFile;
    void	  parseFile(std::ifstream& file);

    /* ---	World	  --- */
    size_t	    world_size;
    Rule	    world_rule;
    uint_ssbo	    alive_cells;

    /* --- OpenGL objects --- */
    Shader          rendering_program;
    GLuint          cubes_vao;
    GLuint	    instance_cube_vertices;

    Shader	    pass_epoch_compute;
    Shader	    gen_pos_buf_compute;
    GLuint	    positions_buffer;

    GLuint	    map3D[2];

    glm::mat4	    world_matrix;
    float	    view_distance;
    glm::mat4	    view_matrix;
    glm::mat4	    proj_matrix;
    GLuint	    mvp_location;    

};
