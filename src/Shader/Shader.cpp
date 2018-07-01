#include <Shader.h>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <iostream>

Shader::Shader() : shaders{0}
{
    program = glCreateProgram();
}

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path)
	    :shaders{0}
{
    program = glCreateProgram();
    loadFromText(vertex_path, fragment_path);
}

Shader::~Shader()
{
    glDeleteProgram(program);
    program = 0;
}

GLuint	    Shader::getProgram()
{
    return program;
}

void	    Shader::use()
{
    glUseProgram(program);
}

void	    Shader::loadFromText(const std::string& file_path, size_t shader_type)
{
    if (shader_type >= NUMBER_OF_SHADER_TYPES){
	return;
    }

    switch(shader_type)
    {
	case VERTEX:	    shaders[shader_type] = glCreateShader(GL_VERTEX_SHADER);	      break;
	case TESS_CONTROL:  shaders[shader_type] = glCreateShader(GL_TESS_CONTROL_SHADER);    break;
	case TESS_EVAL:	    shaders[shader_type] = glCreateShader(GL_TESS_EVALUATION_SHADER); break;
	case GEOMETRY:	    shaders[shader_type] = glCreateShader(GL_GEOMETRY_SHADER);	      break;
	case FRAGMENT:	    shaders[shader_type] = glCreateShader(GL_FRAGMENT_SHADER);	      break;
	case COMPUTE:	    shaders[shader_type] = glCreateShader(GL_COMPUTE_SHADER);	      break;
	// unreachable
	default: return;
    }

    std::string shader_string = readFile(file_path); 
    const GLchar* shader_source = shader_string.c_str();
    glShaderSource(shaders[shader_type], 1, &shader_source, NULL);
    glCompileShader(shaders[shader_type]);

    GLint isCompiled;
    glGetShaderiv(shaders[shader_type], GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
	GLint log_length = 0;
	glGetShaderiv(shaders[shader_type], GL_INFO_LOG_LENGTH, &log_length);
	std::vector<char> error_log(log_length);
	glGetShaderInfoLog(shaders[shader_type], log_length, &log_length, &error_log[0]);
	std::string error_string(error_log.begin(), error_log.end());
	throw std::runtime_error(error_string);
    }

    glAttachShader(program, shaders[shader_type]);
    std::cout << glGetError() << ", : " << shaders[shader_type] << std::endl;
}

void	    Shader::loadFromText(const std::string& vertex_path, const std::string& fragment_path)
{
    loadFromText(vertex_path, VERTEX);
    loadFromText(fragment_path, FRAGMENT);
    link();
}

void	    Shader::link()
{
    glLinkProgram(program);

    GLint isLinked;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

    if (isLinked == GL_FALSE)
    {
	GLint log_length = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
	std::vector<GLchar> error_log(log_length);
	glGetProgramInfoLog(program, log_length, &log_length, &error_log[0]);
	std::string error_string(error_log.begin(), error_log.end());
	throw std::runtime_error(error_string);
    }
} 

void	    Shader::deleteShaders()
{
    for (size_t i = 0; i < NUMBER_OF_SHADER_TYPES; ++i){
	if (shaders[i]){
	   glDeleteShader(shaders[i]);
	   shaders[i] = 0;
	}
    }
}

void	    Shader::setInt(const std::string& name, int num)
{
    glUniform1i(glGetUniformLocation(program, name.c_str()), num);
}

std::string Shader::readFile(const std::string& file_path)
{
    std::ifstream f;
    f.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
	f.open(file_path);
    }catch(std::ifstream::failure e){
	throw;
    }

    return std::string(std::istreambuf_iterator<char>(f),
		       std::istreambuf_iterator<char>());
}
