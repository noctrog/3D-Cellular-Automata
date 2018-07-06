#include <App.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
//#include "stb_image.h"

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cmath>

App::App()
{
    proj_matrix = glm::perspective(70.0f, 1920.0f / 1080.0f, 0.1f, 100.0f);

    setup();
}

App::App(const std::string& _map_file_path) 
    : b_run_single_epoch(false), b_auto_epoch(false), auto_epoch_rate(1.0f), rendering_program()
{
    setup();
    std::ifstream map_file;
    map_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    map_file.open(_map_file_path);

    parseFile(map_file);    
    // TODO: move code from parseFile() and create Shader
}

App::~App()
{
    
}

void  App::parseFile(std::ifstream& file)
{
    std::string ruleString;
    std::getline(file, ruleString);
    ruleString = ruleString.substr(6, 4);
    world_rule.set_rule(ruleString);

    std::string sizeString;
    std::getline(file, sizeString);
    world_size = std::stoi(sizeString.substr(6, std::string::npos));

    std::string currentLine;
    std::getline(file, currentLine);
    if (currentLine != "cells:"){
	throw std::runtime_error("Bad file format");
    }
    else
    {
	// Generate initial world
	auto init_world = std::make_unique<world_unit[]>(std::pow(world_size, 2) *
						    std::ceil(static_cast<float>(world_size) / 8.0f));
	std::vector<glm::ivec3> initial_positions;
	while (std::getline(file, currentLine))
	{
	    // Integer vector to save the positions for the 3D world and compute shader
	    glm::ivec3 currentPos;
	    std::stringstream ss(currentLine);
	    if (ss >> currentPos.x >> currentPos.y >> currentPos.z) break;

	    init_world[std::floor(static_cast<float>(currentPos.x) / 8.0f) + 
		       currentPos.y * world_size +
		       currentPos.z * std::pow(world_size, 2)].cells |= (1 << (7 - currentPos.x % 8));

	    // Float vector to save the positions for drawing the cubes
	    glm::vec3 initial_position (static_cast<float>(currentPos.x),
					static_cast<float>(currentPos.y),
					static_cast<float>(currentPos.z));
	    initial_positions.push_back(initial_position);
	}
	alive_cells.cell_count = initial_positions.size();

	// Create positions buffer
	glGenBuffers(1, &positions_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
	glBufferData(GL_ARRAY_BUFFER, 
		     sizeof(glm::vec3) * initial_positions.size(),
		     initial_positions.data(),
		     GL_STATIC_DRAW);

	// Create world buffer
	glGenBuffers(2, map3D);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, map3D[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER,
		     std::pow(world_size, 2) * std::ceil(static_cast<float>(world_size) / 8.0f),
		     init_world.get(),
		     GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, map3D[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER,
		     std::pow(world_size, 2) * std::ceil(static_cast<float>(world_size) / 8.0f),
		     nullptr,
		     GL_DYNAMIC_DRAW);
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

    glcontext = sdl2::GLContextPtr(new SDL_GLContext(SDL_GL_CreateContext(window.get())));

    SDL_GL_SetSwapInterval(2);
}

void  App::GLinit()
{
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1280, 720);
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
    proj_matrix	  = glm::perspective(70.0f, 1200.0f/720.0f, 100.0f, 0.1f);
    float dist	  = static_cast<float>(world_size) / 2.0f;
    world_matrix  = glm::translate(glm::mat4(1.0f), glm::vec3(-dist));
    view_distance = dist;
    view_matrix	  = glm::lookAt(glm::vec3(view_distance, 0.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));

    static const float    cube_vertices[] = {
	    -0.5f, -0.5f, -0.5f,
	     0.5f, -0.5f, -0.5f,
	     0.5f,  0.5f, -0.5f,
	     0.5f,  0.5f, -0.5f,
	    -0.5f,  0.5f, -0.5f,
	    -0.5f, -0.5f, -0.5f,

	    -0.5f, -0.5f,  0.5f,
	     0.5f, -0.5f,  0.5f,
	     0.5f,  0.5f,  0.5f,
	     0.5f,  0.5f,  0.5f,
	    -0.5f,  0.5f,  0.5f,
	    -0.5f, -0.5f,  0.5f,

	    -0.5f,  0.5f,  0.5f,
	    -0.5f,  0.5f, -0.5f,
	    -0.5f, -0.5f, -0.5f,
	    -0.5f, -0.5f, -0.5f,
	    -0.5f, -0.5f,  0.5f,
	    -0.5f,  0.5f,  0.5f,

	     0.5f,  0.5f,  0.5f,
	     0.5f,  0.5f, -0.5f,
	     0.5f, -0.5f, -0.5f,
	     0.5f, -0.5f, -0.5f,
	     0.5f, -0.5f,  0.5f,
	     0.5f,  0.5f,  0.5f,

	    -0.5f, -0.5f, -0.5f,
	     0.5f, -0.5f, -0.5f,
	     0.5f, -0.5f,  0.5f,
	     0.5f, -0.5f,  0.5f,
	    -0.5f, -0.5f,  0.5f,
	    -0.5f, -0.5f, -0.5f,

	    -0.5f,  0.5f, -0.5f,
	     0.5f,  0.5f, -0.5f,
	     0.5f,  0.5f,  0.5f,
	     0.5f,  0.5f,  0.5f,
	    -0.5f,  0.5f,  0.5f,
	    -0.5f,  0.5f, -0.5f
	};

    glGenVertexArrays(1, &cubes_vao);
    glBindVertexArray(cubes_vao);
    
    glGenBuffers(1, &instance_cube_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, instance_cube_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    rendering_program = std::make_unique<Shader>();
    rendering_program->loadFromText("../src/shaders/cube_vs.glsl", "../src/shaders/cube_fs.glsl");
}


void  App::render()
{
    float background_color[] = { 0.5f + 0.5f*sin(currentTime * 3), 0.0f, 0.0f, 1.0f }; 
    glClearBufferfv(GL_COLOR, 0, background_color);
    glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

    glBindVertexArray(cubes_vao);

    glBindBuffer(GL_ARRAY_BUFFER, instance_cube_vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribDivisor(1, 1);

    glBindVertexArray(cubes_vao);
    rendering_program->use();

    glm::mat4 mvp_matrix = proj_matrix * view_matrix * world_matrix;
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(mvp_matrix));

    glDrawArrays(GL_TRIANGLES, 0, 9);
    //glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 1);
}

void  App::run()
{
    //setup();
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
		    if (e.key.keysym.sym == SDLK_p)	  b_auto_epoch != b_auto_epoch;
		    if (e.key.keysym.sym == SDLK_SPACE)	  b_run_single_epoch = true;
		break;
	    }
	}

	currentTime = static_cast<float>(SDL_GetTicks())/1000.0f;

	//Compute shader
	if (b_auto_epoch || b_run_single_epoch)
	{
	    // magic
	    // passEpoch, and calculate size for positions buffer
	    // convert world to positions buffer
	    
	    b_run_single_epoch = false;
	}

	render();
	SDL_GL_SwapWindow(window.get());
    }

    shutdown();
    terminate();
}

void  App::shutdown()
{
    glDeleteBuffers(1, &instance_cube_vertices);
    glDeleteBuffers(1, &positions_buffer);
    glDeleteBuffers(2, map3D);
    glDeleteVertexArrays(1, &cubes_vao);
}

void  App::terminate()
{
    // Quit OpenGL
    

    // Quit SDL
    SDL_Quit();
}

