#include <cmath>
#include <glm/common.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
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
 
#include "shader.hpp"
#include "texture.hpp"
 
const uint screen_width = 1024;
const uint screen_height = 1024;
 
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

struct slimeAgent
{
    glm::vec4 position; 
    glm::vec4 dir;
};


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 
    GLFWwindow * window = glfwCreateWindow(screen_width, screen_height, "SLIME", NULL, NULL);
 
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window!\n";
        glfwTerminate();
        return -1;
    }
 
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
 
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
    Texture2D screenTex{screen_width, screen_height, 0, GL_RGBA32F};

    auto vertexShader = "assets/vertexShader.glsl";
    auto fragmentShader = "assets/fragmentShader.glsl";
    Shader rectShader(vertexShader, fragmentShader);

    rectShader.set_int("screen", 0);

    screenTex.Activate();

    bool spacePressed = false;

    while (!glfwWindowShouldClose(window))
    {
        countFPS();

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) 
        {
            spacePressed = true;
        }
        else if (spacePressed && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) 
        {
            spacePressed = false;
        }

        // compute shader part

        rectShader.use_shader();
 
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
 
    glfwTerminate();
    return 0;
}
