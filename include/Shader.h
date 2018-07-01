#pragma once 

#include <GL/gl3w.h>
#include <string>
#include <vector>

class Shader
{
public:
    Shader();
    /// Create shader program from vertex and fragment shader
    Shader(const std::string& vertex_path, const std::string& fragment_path);
    virtual ~Shader();

    enum {VERTEX = 0, TESS_CONTROL, TESS_EVAL, GEOMETRY, FRAGMENT, COMPUTE, NUMBER_OF_SHADER_TYPES};

    GLuint	getProgram();
    void	use();
    void	compile();

    /// Loads vertex and fragment shader, attaches to the program and then compiles and links them
    void	loadFromText(const std::string& file_path, size_t shader_type);
    /// Loads a single shader and compiles it. Then attaches it to the program.
    /// It does not compile
    void	loadFromText(const std::string& vertex_path, const std::string& fragment_path);

    void	link();
    void	deleteShaders();

    void	setInt(const std::string& name, int num);
    
private:

    std::string	readFile(const std::string& file_path);


    GLuint program;
    GLuint shaders[NUMBER_OF_SHADER_TYPES];
};
