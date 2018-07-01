#pragma once

#include <GL/gl3w.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <Shader.h>

class App
{
public:
    App ();
    virtual ~App ();

    /// Runs at the beginning, set up SDL and OpenGL
    virtual void setup();

    /// runs once right after setup
    virtual void startup();

    /// The program that runs each frame
    virtual void render();

    /// Executes everything in order: setup, startup, render loop, shutdown 
    virtual void run();

    /// Runs after render loop
    virtual void shutdown();

    /// Closes SDL and OpenGL, frees memory
    virtual void terminate();

private:
    void SDLinit();
    void GLinit();

    void UpdateTime();

    SDL_Window* window;
    SDL_GLContext glcontext;

    /// Saves the current value of time, in seconds
    float currentTime;

    /* --- OpenGL objects --- */
    Shader*         program;
    GLuint          VAO;
    GLuint	    VBO, EBO;

    GLint           mv_location;
    GLint           proj_location;

    GLuint	    map3D;
    GLuint	    map3D_aux;

    float           aspect;
    glm::mat4	    proj_matrix;
   };
