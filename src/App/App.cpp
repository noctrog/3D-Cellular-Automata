#include <App.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
//#include "stb_image.h"

#include <exception>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

std::string readShaderFromFile(const char* filename)
{
    std::ifstream glsl_shader;
    try
    {
	glsl_shader.open(filename);
    }catch(...){
	throw std::runtime_error(std::string("Error al abrir el archivo ") + filename);
    }

    return std::string(std::istreambuf_iterator<char>(glsl_shader),
		       std::istreambuf_iterator<char>());
}

App::App()
{
    proj_matrix = glm::perspective(70.0f, 1920.0f / 1080.0f, 0.1f, 100.0f);
}

App::~App()
{
    
}

void  App::SDLinit()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0){
	throw std::runtime_error(SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
 
    window.reset(SDL_CreateWindow("App", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_OPENGL));
    if (window == nullptr){
	throw std::runtime_error(SDL_GetError());
    }

    std::cout << "Se ha pasado de glcontext" << std::endl;
    //glcontext = std::make_unique<SDL_GLContext, sdl2::SDL_Delete>(SDL_GL_CreateContext(window.get()));
    glcontext = sdl2::GLContextPtr(new SDL_GLContext(SDL_GL_CreateContext(window.get())));

    SDL_GL_SetSwapInterval(20);
}

void  App::GLinit()
{
    glEnable(GL_DEPTH_TEST);
    //std::cout << glGetString(GL_VERSION) << std::endl;
    glViewport(0, 0, 1280, 720);
    GLint data = 1;
    glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &data);
    std::cout << "OpenGL max compute work group size: " << data << std::endl;
}

void  App::setup()
{
   // Init SDL
   SDLinit();
   // Init OpenGL
   GLinit();
}

void  App::startup()
{

}


void  App::render()
{

}

void  App::run()
{
    setup();
    startup();

    bool running = true;
    SDL_Event e;
    while (running)
    {
	while(SDL_PollEvent(&e))
	{
	    switch (e.type)
	    {
		case SDL_KEYDOWN:
		    if (e.key.keysym.sym == SDLK_ESCAPE) running = false;
	    }
	}

	currentTime = static_cast<float>(SDL_GetTicks())/1000.0f;

	render();
	SDL_GL_SwapWindow(window.get());

	SDL_Delay(10);
    }

    shutdown();
    terminate();
}

void  App::shutdown()
{

}

void  App::terminate()
{
    // Quit OpenGL
    

    // Quit SDL
    SDL_Quit();
}

