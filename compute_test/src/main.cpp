#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <random>
 
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
 
#include "helpers/RootDir.hpp"

#include "shader.hpp"
 
const uint screen_width = 1000;
const uint screen_height = 1000;
 
int num_frames{ 0 };
float last_time{ 0.0f };
 
GLfloat vertices[] =
{
	-1.0f, -1.0f , 0.0f, 0.0f, 1.0f,
	-1.0f,  1.0f , 0.0f, 0.0f, 0.0f,
	 1.0f,  1.0f , 0.0f, 1.0f, 0.0f,
	 1.0f, -1.0f , 0.0f, 1.0f, 1.0f,
};

GLuint indices[] =
{
	0, 2, 1,
	0, 3, 2
};

void countFPS()
{
    double current_time = glfwGetTime();
    num_frames++;
    if (current_time - last_time >= 1.0)
    {
        std::cout << 1000.0 / num_frames << "ms / frame\n";
        num_frames = 0;
        last_time += 1.0;
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 
    GLFWwindow * window = glfwCreateWindow(screen_width, screen_height, "ComputeShaders", NULL, NULL);
 
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window!\n";
        glfwTerminate();
        return -1;
    }
 
    glfwMakeContextCurrent(window);
 
    if (glewInit())
    {
        std::cout << "Failed initializing GLEW\n";
    }
 
    glViewport(0, 0, screen_width, screen_height);
 
    last_time = glfwGetTime();
    GLuint VAO, VBO, EBO;
	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);

	glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glNamedBufferData(EBO, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));

	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(GLfloat));
	glVertexArrayElementBuffer(VAO, EBO);

    // compute shader texture

    GLuint screenTex;
    glCreateTextures(GL_TEXTURE_2D, 1, &screenTex);
    glTextureParameteri(screenTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(screenTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(screenTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(screenTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(screenTex, 1, GL_RGBA32F, screen_width, screen_height);
    glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    GLuint map;
    glCreateTextures(GL_TEXTURE_2D, 1, &map);    
    glTexParameteri(map, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(map, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(map, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(map, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureStorage2D(map, 1, GL_RGBA32F, screen_width, screen_height);
    glBindImageTexture(1, map, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    std::string charMap[10] {"##########",
                             "#        #",
                             "#        #",
                             "#  #  #  #",
                             "#        #",
                             "# #    # #",
                             "#  #  #  #",
                             "#   ##   #",
                             "##       #",
                             "##########"};

    GLubyte data[screen_height][screen_width][4] = {};
    int last = -1;
    for (size_t y = 0; y < screen_height; ++y) 
    {
        for (size_t x = 0; x < screen_width; ++x) 
        {
            int index_y = y/100;
            int index_x = x/100;

            if (charMap[index_y][index_x] == '#') 
            {
                data[y][x][0] = (GLubyte)255;
                data[y][x][1] = (GLubyte)255;
                data[y][x][2] = (GLubyte)255;
                data[y][x][3] = (GLubyte)255;
            }
            else
            {
                data[y][x][0] = 0;
                data[y][x][1] = 0;
                data[y][x][2] = 0;
                data[y][x][3] = (GLubyte)255;
            }
        }
    }

    glTextureSubImage2D(map, 0, 0, 0, screen_width, screen_height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    std::string root{ROOT_DIR};
    auto vertexShader = root+"shaders/vertexShader.glsl";
    auto fragmentShader = root+"shaders/fragmentShader.glsl";
    Shader rectShader(vertexShader.c_str(), fragmentShader.c_str());

    struct agent
    {
        glm::vec4 position; 
        glm::vec4 color;
    };

    int agentSize = 1000000;
    std::vector<agent> agentVector;
    for (size_t x = 0; x < agentSize; ++x) 
    {
        auto randomOffset = glm::diskRand(screen_width/3.0f);
        auto randomColor = glm::ballRand(255.0f);

        agentVector.push_back(
                agent{
                glm::vec4{screen_width/2.0f + randomOffset.x, screen_height/2.0f + randomOffset.y,0,0},
                glm::vec4{randomColor.x,randomColor.y,randomColor.z,1}
                }
            );
    }

    GLuint agentSSbo;

    glGenBuffers(1, &agentSSbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, agentSSbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, agentSize*sizeof(agent), &agentVector[0], GL_STATIC_DRAW);

    auto computeShaderPath = root+"shaders/computeShader.glsl";
    Compute computeShader(computeShaderPath.c_str());

    rectShader.set_int("screen", 0);
    rectShader.set_int("map", 1);

    computeShader.set_int("screen", 0);
    computeShader.set_int("map", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTex);

    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, map);

    uint blank = 0;
    while (!glfwWindowShouldClose(window))
    {
        countFPS();

        // compute shader part

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, agentSSbo);
        computeShader.use_shader(agentSize/32,1,1);
        computeShader.wait();

        // drawing part

        rectShader.use_shader();
 
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &agentSSbo);
 
    glfwTerminate();
    return 0;
}
