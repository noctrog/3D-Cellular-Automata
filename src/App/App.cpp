#include <App.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
//#include "stb_image.h"

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cmath>

#include <bitset>

#include <stdio.h>
static void APIENTRY simple_print_callback( GLenum source,
					    GLenum type,
					    GLuint id,
					    GLenum severity,
					    GLsizei length,
					    const GLchar* message,
					    const void* userParam)
{
    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
	printf( "Debug message with source 0x%04X, type 0x%04X, "
		"id %u, severity 0x%0X, `%s`\n",
		source, type, id, severity, message);
}

App::App()
{
    proj_matrix = glm::perspective(70.0f, 1920.0f / 1080.0f, 0.1f, 100.0f);

    setup();
}

App::App(const std::string& _map_file_path) 
    : the_world(_map_file_path), b_run_single_epoch(false), b_auto_epoch(false), auto_epoch_rate(1.0f), cam_angle(0), even_epoch(false)
{
    // Setup SDL2 and OpenGL
    setup();
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
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
 
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
    glDepthFunc(GL_GEQUAL);
    glViewport(0, 0, 1280, 720);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);

    //glEnable(GL_MULTISAMPLE);
    //
    glDebugMessageCallback(&simple_print_callback, NULL);
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
    proj_matrix	      = glm::perspective(70.0f, 1200.0f/720.0f, 100.0f, 0.1f);
    float dist	      = static_cast<float>(world_size);
    world_matrix      = glm::translate(glm::mat4(1.0f), glm::vec3(-dist/2.0f));
    view_distance     = dist;
    view_matrix	      = glm::lookAt(glm::vec3(view_distance * cos(cam_angle),
					      view_distance * sin(cam_angle),
					      0.0f),
				    glm::vec3(0.0f, 0.0f, 0.0f),
				    glm::vec3(0.0f, 0.0f, 1.0f));

    static const GLfloat    cube_vertices[] = {
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

    // Create positions buffer
    glCreateBuffers(1, &positions_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
    glBufferData(GL_ARRAY_BUFFER, 
		 the_world.get_positions_buffer_size(),
		 the_world.get_positions_buffer(),
		 GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create vertex array object for rendering the cubes 
    glCreateVertexArrays(1, &cubes_vao);
    glBindVertexArray(cubes_vao);
    
    // Create buffer holding one cube's vertex
    glCreateBuffers(1, &instance_cube_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, instance_cube_vertices);
    glBufferData(GL_ARRAY_BUFFER, 
		 sizeof(cube_vertices), 
		 cube_vertices,
		 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glBindVertexArray(0);

    rendering_program = std::make_unique<Shader>();
    rendering_program->loadFromText("../src/shaders/cube_vs.glsl",
				    "../src/shaders/cube_fs.glsl");
}

void  App::render()
{
    float background_color[] = { 0.5f + 0.1f*sin(currentTime), 0.0f, 0.5f, 1.0f }; 
    glClearBufferfv(GL_COLOR, 0, background_color);
    static const float zero = 0.0f;
    glClearBufferfv(GL_DEPTH, 0, &zero);

    // Update VAO to read from the new positions buffer
    glBindVertexArray(cubes_vao);
    glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    rendering_program->use();

    glm::mat4 mvp_matrix = proj_matrix * view_matrix * world_matrix;
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp_matrix));

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, alive_cells);
}

void  App::run()
{
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

		    if (e.key.keysym.sym == SDLK_a)	  cam_angle -= 0.1f;
		    if (e.key.keysym.sym == SDLK_d)	  cam_angle += 0.1f;
		    if (e.key.keysym.sym == SDLK_w)	  view_distance -= 5.0f;
		    if (e.key.keysym.sym == SDLK_s)	  view_distance += 5.0f;
		break;
	    }
	}

	currentTime = static_cast<float>(SDL_GetTicks())/1000.0f;

	//Compute shader
	// TODO: auto epoch time delay
	if (b_auto_epoch || b_run_single_epoch)
	{
	    the_world.evolve();
	    b_run_single_epoch = false;
	}
	
        view_matrix = glm::lookAt(glm::vec3(view_distance * cos(cam_angle), 0.0f, view_distance * sin(cam_angle)),
		      glm::vec3(0.0f, 0.0f, 0.0f),
		      glm::vec3(0.0f, 1.0f, 0.0f));
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

