#include "shader.hpp"
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

GLfloat vertices[] = 
{
    // positions
    1.0f,  1.0f, 0.0f,    // top right
    1.0f, -1.0f, 0.0f,    // bottom right
    -1.0f, -1.0f, 0.0f,   // bottom left
    -1.0f,  1.0f, 0.0f,   // top left 
};
 
GLuint indices[] = {  
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

void handleInputEvents(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
    {
        glfwSetWindowShouldClose(window, 1);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) 
    {
        glClearColor(0,1,0,1);
    }
}
 
int main(int argc, char *argv[])
{
    /* Initialize the library */
    if (!glfwInit())
    {
        std::cout << "GLFW init fail" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(800, 800, "GLFW Raymarcher1", NULL, NULL);
    if (!window)
    {
        std::cout << "GLFW window create fail" << std::endl;
        glfwTerminate();
        return -1;
    }


    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (glewInit())
    {
        std::cout << "Failed initializing GLEW" << std::endl;;
    }

    glViewport(0,0,800,800);

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

    glm::vec3 cameraOrigin{0,0,-3};

    Shader our_shader("./src/shaders/vertexShader.glsl", "./src/shaders/fragmentShader.glsl");
    our_shader.use_shader();
    our_shader.set_vec2("resolution", {800,800});
    our_shader.set_vec3("cameraOrigin", cameraOrigin);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    while (!glfwWindowShouldClose(window))
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // *** rendering ***
        our_shader.use_shader();

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);

        glfwPollEvents();
        handleInputEvents(window);
    }

    glfwTerminate();
    return 0;
}
