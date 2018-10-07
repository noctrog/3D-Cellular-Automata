#pragma once

#include <memory>
#include <string>
#include <fstream>

#include <GL/gl3w.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <Shader.h>
#include <World3d.h>

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

    sdl2::WindowPtr	      window;
    sdl2::GLContextPtr	      glcontext;

    /// Saves the current value of time, in seconds
    float currentTime;

    /* ---   User input   --- */
    bool		      b_run_single_epoch;
    bool		      b_auto_epoch;
    float		      auto_epoch_rate;

    /* --- World --- */
    World3d the_world;

    /* --- OpenGL objects --- */
    std::unique_ptr<Shader>   rendering_program;
    GLuint		      cubes_vao;
    GLuint		      instance_cube_vertices;

    float		      cam_angle;

    GLuint		      positions_buffer;

    /* --- Camera properties --- */
    glm::mat4		      world_matrix;
    float		      view_distance;
    glm::mat4		      view_matrix;
    glm::mat4		      proj_matrix;
    GLuint		      mvp_location;    
};
