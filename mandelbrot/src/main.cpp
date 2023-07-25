#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <string>
 
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
 
#include "helpers/RootDir.hpp"

#include "shader.hpp"
 
int screen_width{ 1000 };
int screen_height{ 1000 };
 
int num_frames{ 0 };
float last_time{ 0.0f };
 
GLfloat vertices[] = 
{
    // positions
    1.0f,  1.0f, 0.0f,    // top right
    1.0f, -1.0f, 0.0f,    // bottom right
    -1.0f, -1.0f, 0.0f,   // bottom left
    -1.0f,  1.0f, 0.0f,   // top left 
    // UV
    /* 1.0f, 1.0f, */
    /* 1.0f, 0.0f, */
    /* 0.0f, 0.0f, */
    /* 0.0f, 1.0f, */
};
 
GLuint indices[] = {  
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 
    GLFWwindow * window = glfwCreateWindow(screen_width, screen_height, "Mandelbrot", NULL, NULL);
 
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
 
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
 
    glBindVertexArray(VAO);
 
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::string root{ROOT_DIR};
    auto vertexShader = root+"shaders/vertexShader.glsl";
    auto fragmentShader = root+"shaders/fragmentShader.glsl";

    Shader our_shader(vertexShader.c_str(), fragmentShader.c_str());

    glm::vec3 cameraPos{-0.7, -0.5, 2};
    int maxIter = 50;

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
        countFPS();

        int leftRight = 0;
        int upDown    = 0;
        int zoom      = 0;
        int iter      = 0;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
        {
            leftRight += -1;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
        {
            leftRight += +1;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
        {
            upDown += +1;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
        {
            upDown += -1;
        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) 
        {
            zoom += -1;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) 
        {
            zoom += +1;
        }

        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) 
        {
            iter += -1;
        }
        if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) 
        {
            iter += +1;
        }

        cameraPos += glm::vec3{0.005*cameraPos.z*leftRight, 0.005*cameraPos.z*upDown, zoom*0.01*cameraPos.z};
        maxIter += iter;
 
        our_shader.use_shader();

        our_shader.set_vec3("camera", cameraPos);
        our_shader.set_int("MAX_ITERATIONS", maxIter);
        
 
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
