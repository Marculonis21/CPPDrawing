#ifndef _SHADER_HPP
#define _SHADER_HPP 

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
 
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
 
class Shader
{
public:
    unsigned int program_ID;
    Shader();
    ~Shader();

    void addShader(const char *shaderPath, GLenum shaderType);
    void linkProgram();

    void useShader();
 
    void set_int(const std::string & name, int value) const;
    void set_float(const std::string & name, float value) const;
    void set_vec3(const std::string & name, glm::vec3 vec) const;
    void set_vec4(const std::string & name, glm::vec4 vec) const;
 
private:
    std::vector<GLuint> shaders;

    bool linked=false;

    bool checkShaderCode(GLuint shader, const std::string & shaderPath);
    std::string read_shader_file(const char * file_path);
    void add_shader(unsigned int program, const char * shader_path, GLenum shader_type);
};

class Compute
{
public:
    unsigned int computeProgram;
    Compute(const char * compute_shader_path);
    ~Compute();
 
    void use_shader(GLuint workGroup_X, GLuint workGroup_Y, GLuint workGroup_Z);
    void wait();

    void set_int(const std::string & name, int value) const;
 
private:
    std::string read_shader_file(const char * file_path);
    void add_shader(unsigned int shader, const char * shader_path);
};

#endif
