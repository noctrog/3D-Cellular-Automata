#include <App.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
//#include "stb_image.h"

#include <stdexcept>
#include <iostream>
#include <cmath>

App::App()
{
    proj_matrix = glm::perspective(70.0f, 1920.0f / 1080.0f, 0.1f, 100.0f);
}

App::App(const std::string& _mapFilePath) 
    : bRunSingleEpoch(false), bAutoEpoch(false), autoEpochRate(1.0f)
{
    std::ifstream mapFile;
    mapFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    mapFile.open(_mapFilePath);

    
}

App::~App()
{
    
}

void  App::parseFile(std::ifstream& file)
{
    std::string ruleString;
    std::getline(file, ruleString);
    ruleString = ruleString.substr(6, 4);
    worldRule.set_rule(ruleString);

    std::string sizeString;
    std::getline(file, sizeString);
    worldSize = std::stoi(sizeString.substr(6, std::string::npos));

    std::string currentLine;
    std::getline(file, currentLine);
    if (currentLine != "cells:"){
	throw std::runtime_error("Bad file format");
    }
    else
    {
	// Generate initial world
	auto init_world = std::make_unique<world_unit[]>(std::pow(worldSize, 2) *
						    std::ceil(static_cast<float>(worldSize) / 8.0f));

	while (std::getline(file, currentLine))
	{
	    glm::ivec3 currentPos;
	    currentPos.x = std::stoi(std::strtok(&currentLine[0], " ,"));
	    currentPos.y = std::stoi(std::strtok(nullptr, " ,"));
	    currentPos.z = std::stoi(std::strtok(nullptr, " ,"));

	    init_world[std::floor(static_cast<float>(currentPos.x) / 8.0f) + 
		       currentPos.y * worldSize +
		       currentPos.z * std::pow(worldSize, 2)].cells |= (1 << (7 - currentPos.x % 8));
	}

	// Create world buffer
	glGenBuffers(2, map3D);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, map3D[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER,
		     std::pow(worldSize, 2) * std::ceil(static_cast<float>(worldSize) / 8.0f),
		     init_world.get(),
		     GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, map3D[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER,
		     std::pow(worldSize, 2) * std::ceil(static_cast<float>(worldSize) / 8.0f),
		     nullptr,
		     GL_DYNAMIC_COPY);
    }

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
    glcontext = sdl2::GLContextPtr(new SDL_GLContext(SDL_GL_CreateContext(window.get())));

    SDL_GL_SetSwapInterval(20);
}

void  App::GLinit()
{
    glEnable(GL_DEPTH_TEST);
    std::cout << glGetString(GL_MAX_TEXTURE_BUFFER_SIZE) << std::endl;
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
		    if (e.key.keysym.sym == SDLK_ESCAPE)  running = false;
		    if (e.key.keysym.sym == SDLK_p)	  bAutoEpoch != bAutoEpoch;
		    if (e.key.keysym.sym == SDLK_SPACE)	  bRunSingleEpoch = true;
	    }
	}

	currentTime = static_cast<float>(SDL_GetTicks())/1000.0f;

	//Compute shader
	if (bAutoEpoch || bRunSingleEpoch)
	{
	    // magic
	    // passEpoch, and calculate size for positions buffer
	    // convert world to positions buffer
	    
	    bRunSingleEpoch = false;
	}

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

