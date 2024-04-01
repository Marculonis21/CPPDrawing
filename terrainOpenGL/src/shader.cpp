#include "shader.hpp"
#include <stdexcept>

Shader::Shader()
{
    this->program_ID = glCreateProgram();
}

Shader::~Shader()
{
    for (int i = 0; i < shaders.size(); ++i) {
        glDeleteShader(shaders[i]);
    }
    glDeleteProgram(program_ID);
}

bool Shader::checkShaderCode(GLuint shader, const std::string & shaderPath)
{
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char error_message[512];

        glGetShaderInfoLog(shader, 512, nullptr, error_message);
        std::cout << "Error compiling shader: " << error_message << "\n";
        std::cout << "Shader location: " << shaderPath << "\n";
        return false;
    }
    return true;
}

void Shader::addShader(const char *shaderPath, GLenum shaderType)
{
    if (linked) {
        throw std::invalid_argument("Adding to shader after linking not possible!");
    }

    std::string shaderCode = read_shader_file(shaderPath);
    const GLchar *code[1] {shaderCode.c_str()}; 

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, code, NULL);
    glCompileShader(shader);

    if (!checkShaderCode(shader, shaderPath)) 
    {
        throw std::invalid_argument("Encountered incorrect shader code");
    }

    glAttachShader(program_ID, shader);
    shaders.push_back(shader);
}

void Shader::linkProgram()
{
    linked = true;

    glLinkProgram(program_ID);
}

void Shader::useShader()
{
    if (!linked) {
        throw std::invalid_argument("Shader not linked before use");
    }

    glUseProgram(program_ID);
}

std::string Shader::read_shader_file(const char * file_path)
{
    std::string code;
    std::ifstream shader_file(file_path, std::ios::in);
 
    if (!shader_file)
    {
        std::cout << "Failed to open shader file: " << file_path << "\n";
        return "";
    }
 
    std::string line = "";
    while (std::getline(shader_file, line))
    {
        code.append(line + '\n');
    }
         
    shader_file.close();
    return code;
}

void Shader::add_shader(unsigned int program, const char * shader_path, GLenum shader_type)
{
    std::string shader_string = read_shader_file(shader_path);
 
    const GLchar *code[1];
    code[0] = shader_string.c_str();
 
    GLint code_length[1];
    code_length[0] = strlen(shader_string.c_str());
 
    unsigned int shader;
    int success;
    char error_message[512];
 
    shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, code, code_length);
    glCompileShader(shader);
 
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, error_message);
        std::cout << "Error compiling shader: " << error_message << "\n";
        std::cout << "Shader location: " << shader_path << "\n";
    }
 
    glAttachShader(program, shader);
}

void Shader::set_int(const std::string & name, int value) const
{
    glUniform1i(glGetUniformLocation(program_ID, name.c_str()), value);
}

void Shader::set_float(const std::string & name, float value) const
{
    glUniform1f(glGetUniformLocation(program_ID, name.c_str()), value);
}

void Shader::set_vec3(const std::string & name, glm::vec3 vec) const
{
    glUniform3f(glGetUniformLocation(program_ID, name.c_str()), vec.x, vec.y, vec.z);
}

void Shader::set_vec4(const std::string & name, glm::vec4 vec) const
{
    glUniform4f(glGetUniformLocation(program_ID, name.c_str()), vec.x, vec.y, vec.z, vec.w);
}

void Shader::set_mat4(const std::string & name, glm::mat4 mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(program_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

Compute::Compute(const char * compute_shader_path)
{
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    add_shader(computeShader, compute_shader_path);

    computeProgram = glCreateProgram();
 
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);
 
    int success;
    char error_message[512];
    glGetProgramiv(computeProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(computeProgram, 512, nullptr, error_message);
        std::cout << "Error linking shader program: " << error_message << "\n";
    }
}


void Compute::add_shader(unsigned int shader, const char * shader_path)
{
    std::string shader_string = read_shader_file(shader_path);
 
    const GLchar * code[1];
    code[0] = shader_string.c_str();
 
    GLint code_length[1];
    code_length[0] = strlen(shader_string.c_str());
 
    int success;
    char error_message[512];
 
    glShaderSource(shader, 1, code, code_length);
    glCompileShader(shader);
 
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, error_message);
        std::cout << "Error compiling shader: " << error_message << "\n";
        std::cout << "Shader location: " << shader_path << "\n";
    }
}

void Compute::use_shader(GLuint workGroup_X, GLuint workGroup_Y, GLuint workGroup_Z)
{
    glUseProgram(computeProgram);
    glDispatchCompute(workGroup_X, workGroup_Y, workGroup_Z);
}

void Compute::wait()
{
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

std::string Compute::read_shader_file(const char * file_path)
{
    std::string code;
    std::ifstream shader_file(file_path, std::ios::in);
 
    if (!shader_file)
    {
        std::cout << "Failed to open shader file: " << file_path << "\n";
        return "";
    }
 
    std::string line = "";
    while (std::getline(shader_file, line))
    {
        code.append(line + '\n');
    }
         
    shader_file.close();
    return code;
}

Compute::~Compute()
{
    if (computeProgram != 0)
    {
        glDeleteProgram(computeProgram);
        computeProgram = 0;
    }
}

void Compute::set_int(const std::string & name, int value) const
{
    glUniform1i(glGetUniformLocation(computeProgram, name.c_str()), value);
}
