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
    : b_run_single_epoch(false), b_auto_epoch(false), auto_epoch_rate(1.0f), cam_angle(0), even_epoch(false),
      alive_cells(0)
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
	// Create world buffer
	glCreateBuffers(2, map3D);
	std::vector<uint32_t> init_world(std::pow(world_size, 2) * std::ceil(static_cast<float>(world_size) / 32.0f), 0);

	// Save the auxiliary map initialized to 0s
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, map3D[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER,
		     init_world.size() * sizeof(uint32_t),
		     init_world.data(),
		     GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	uint32_t n_cells_per_row = std::ceil(static_cast<float>(world_size) / 32.0f);
	std::vector<glm::vec3> initial_positions;
	while (std::getline(file, currentLine))
	{
	    // Integer vector to save the positions for the 3D world and compute shader
	    glm::ivec3 current_pos;
	    std::stringstream ss(currentLine);
	    if (!(ss >> current_pos.x >> current_pos.y >> current_pos.z)) break;

	    // Save positions as floats for the positions buffer
	    glm::vec3 current_pos_f (static_cast<float>(current_pos.x),
				     static_cast<float>(current_pos.y),
				     static_cast<float>(current_pos.z));
	    initial_positions.push_back(current_pos_f);

	    init_world[	std::floor(current_pos_f.x / 32.0f)	    + 
			n_cells_per_row * current_pos.y		    +
			world_size * n_cells_per_row * current_pos.z] |= (1 << (31 - current_pos.x % 32));

	    std::cout << "X: " << initial_positions.back().x << 
			" Y: " << initial_positions.back().y <<
			" Z: " << initial_positions.back().z << std::endl;
	}
	alive_cells = initial_positions.size();

	// Create positions buffer
	glCreateBuffers(1, &positions_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
	glBufferData(GL_ARRAY_BUFFER, 
		     sizeof(glm::vec3) * initial_positions.size(),
		     glm::value_ptr(initial_positions[0]),
		     GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Save initial map in GPU
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, map3D[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER,
		     init_world.size() * sizeof(uint32_t),
		     init_world.data(),
		     GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	// Print ouput_map buffer
	std::cout << "--- map3D[0] ---" << std::endl;
	uint32_t* tmp = (uint32_t*) glMapNamedBuffer(map3D[0], GL_READ_ONLY);
	for (size_t i = 0; i < world_size; ++i){
	    for (size_t j = 0; j < world_size; ++j){
		for (size_t k = 0; k < std::ceil(static_cast<float>(world_size) / 32.0f); ++k){
		    std::cout << std::bitset<32>(*(tmp++)) << " ";
		}
		std::cout << std::endl;
	    }
	    std::cout << std::endl << std::endl;
	}
	glUnmapNamedBuffer(map3D[0]);

	std::cout << "--- map3D[1] ---" << std::endl;
	tmp = (uint32_t*) glMapNamedBuffer(map3D[1], GL_READ_ONLY);
	for (size_t i = 0; i < world_size; ++i){
	    for (size_t j = 0; j < world_size; ++j){
		for (size_t k = 0; k < n_cells_per_row; ++k){
		    std::cout << std::bitset<32>(*(tmp++)) << " ";
		}
		std::cout << std::endl;
	    }
	    std::cout << std::endl << std::endl;
	}
	glUnmapNamedBuffer(map3D[1]);
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
    view_matrix	      = glm::lookAt(glm::vec3(view_distance * cos(cam_angle), view_distance * sin(cam_angle),0.0f),
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

    glCreateVertexArrays(1, &cubes_vao);
    glBindVertexArray(cubes_vao);
    
    glCreateBuffers(1, &instance_cube_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, instance_cube_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glBindVertexArray(0);

    glCreateBuffers(1, &alive_cells_atc);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, alive_cells_atc);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);


    rendering_program = std::make_unique<Shader>();
    rendering_program->loadFromText("../src/shaders/cube_vs.glsl", "../src/shaders/cube_fs.glsl");

    pass_epoch_compute = std::make_unique<Shader>();
    pass_epoch_compute->loadFromText("../src/shaders/epoch_compute.glsl", Shader::COMPUTE);
    pass_epoch_compute->link();

    gen_pos_buf_compute = std::make_unique<Shader>();
    gen_pos_buf_compute->loadFromText("../src/shaders/gen_pos_compute.glsl", Shader::COMPUTE);
    gen_pos_buf_compute->link();
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
	if (b_auto_epoch || b_run_single_epoch)
	{
	    /*
	     *	  First stage
	     *    Read world, apply rule to every cell and update it
	     *    Also, count how many cells are alive in new epoch
	     */

	    // Select input and output map and bind them
	    GLuint input_world = 0, output_world = 0;
	    if (even_epoch) { input_world = map3D[0]; output_world = map3D[1];	}
	    else	    { input_world = map3D[1]; output_world = map3D[0];	}
	    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, input_world);
	    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, output_world);

	    // Set cell counter to 0 and bind it
	    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, alive_cells_atc);
	    uint32_t zero = 0;
	    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
	    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, alive_cells_atc);

	    pass_epoch_compute->use();

	    glUniform1ui(0, world_size);
	    glUniform4uiv(1, 1, world_rule.get_rule().data());
	    // Change even to odd and vice_versa
	    even_epoch = !even_epoch;
	    
	    // Calculate how much compute groups are needed and execute
	    uint32_t numWorkGroups = std::ceil(static_cast<float>(world_size) / 10.0f);
	    //std::cout << numWorkGroups << std::endl;
	    glDispatchCompute(numWorkGroups, numWorkGroups, numWorkGroups);

	    glMemoryBarrier(GL_ALL_BARRIER_BITS);

	    // Get the total amount of alive cells (held by the atomic counter)
	    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, alive_cells_atc);
	    GLuint* alive_cells_data = (GLuint*) glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER,
								  0,
								  sizeof(uint32_t),
								  GL_MAP_READ_BIT); 
	    alive_cells = alive_cells_data[0];
	    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	    //std::cout << "OpenGL error: " << glGetError() << std::endl;
	    std::cout << "Cells: " << alive_cells << std::endl;

	    // Print ouput_map buffer
	    std::cout << "--- Output world ---" << std::endl;
	    uint32_t* tmp = (uint32_t*) glMapNamedBuffer(output_world, GL_READ_ONLY);
	    for (size_t i = 0; i < world_size; ++i){
		for (size_t j = 0; j < world_size; ++j){
		    for (size_t k = 0; k < std::ceil(static_cast<float>(world_size) / 32.0f); ++k){
			std::cout << std::bitset<32>(*(tmp++)) << " ";
		    }
		    std::cout << std::endl;
		}
		std::cout << std::endl;
	    }
	    glUnmapNamedBuffer(output_world);

	    // Print aux map 
	    std::cout << "--- Auxiliary world ---" << std::endl;
	    tmp = (uint32_t*) glMapNamedBuffer(input_world, GL_READ_ONLY);
	    for (size_t i = 0; i < world_size; ++i){
		for (size_t j = 0; j < world_size; ++j){
		    for (size_t k = 0; k < std::ceil(static_cast<float>(world_size) / 32.0f); ++k){
			std::cout << std::bitset<32>(*(tmp++)) << " ";
		    }
		    std::cout << std::endl;
		}
		std::cout << std::endl;
	    }
	    glUnmapNamedBuffer(input_world);
	    /*
	     *	Second stage
	     *	Reallocate positions_buffer and from the new world generated,
	     *	fill the new positions_buffer (needed by glDrawInstanced)
	     */

	    // Bind map
	    glBindBuffer(GL_SHADER_STORAGE_BUFFER, output_world);
	    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, output_world);

	    // Create new positions buffer
	    std::vector<GLfloat> zeros(alive_cells * 3, 0.0f);
	    glDeleteBuffers(1, &positions_buffer);
	    glCreateBuffers(1, &positions_buffer);
	    glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
	    glBufferData(GL_ARRAY_BUFFER, 
			 3 * sizeof(GL_FLOAT) * alive_cells,
			 zeros.data(), 
			 GL_DYNAMIC_COPY);
	    glBindBuffer(GL_ARRAY_BUFFER, 0);

	    // Bind positions buffer to the shader
	    glBindBuffer(GL_SHADER_STORAGE_BUFFER, positions_buffer);
	    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, positions_buffer);
	    
	    // The atomic counter now serves as an index to access the positions buffer
	    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, alive_cells_atc);
	    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
	    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, input_world);

	    gen_pos_buf_compute->use();

	    // Set the map size
	    glUniform1ui(0, world_size);	    

	    //glDispatchCompute(numWorkGroups, numWorkGroups, numWorkGroups);

	    glMemoryBarrier(GL_ALL_BARRIER_BITS);

	    b_run_single_epoch = false;

	    //float* tmp = (float*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
	    ///[>tmp = 7.0f; *(tmp + 1) = 7.0f; *(tmp + 2) = 7.0f;
	    //for (size_t i = 0; i < alive_cells; ++i){
		//std::cout << "Nueva pos: " << *(tmp++) << ", " << *(tmp++) << ", " << *(tmp++) << std::endl;
	    //}
	    //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
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

