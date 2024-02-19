#include "shader.hpp"
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <string>

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

glm::vec3 cameraOrigin{0,0,-3};
glm::vec3 cameraLookAt{1,0,0};
glm::vec2 cameraRotation{0,90};

double last_mouseXPos=400;
double last_mouseYPos=400;

void handleInputEvents(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
    {
        glfwSetWindowShouldClose(window, 1);
    }

    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
    {
        cameraOrigin.z += 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
    {
        cameraOrigin.z -= 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
    {
        cameraOrigin.x -= 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
    {
        cameraOrigin.x += 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) 
    {
        cameraOrigin.y += 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) 
    {
        cameraOrigin.y -= 0.01;
    }

    // Camera rotation
    double xPos;
    double yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    glm::vec2 delta{last_mouseXPos-xPos, last_mouseYPos-yPos};
    last_mouseXPos = xPos;
    last_mouseYPos = yPos;

    cameraRotation.x -= delta.x;
    cameraRotation.y -= delta.y;

    auto _cameraRotation = glm::radians(cameraRotation);

    // https://www.mathforengineers.com/math-calculators/spherical-to-rectangular-coordinates.html
    cameraLookAt = glm::vec3(glm::sin(_cameraRotation.y)*glm::cos(_cameraRotation.x),
                             glm::cos(_cameraRotation.y),
                             glm::sin(_cameraRotation.y)*glm::sin(_cameraRotation.x));

    cameraLookAt += cameraOrigin;
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

    Shader our_shader("./src/shaders/vertexShader.glsl", "./src/shaders/fragmentShader.glsl");
    our_shader.use_shader();
    our_shader.set_vec2("resolution", {800,800});

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // *** rendering ***
        our_shader.use_shader();
        our_shader.set_vec3("cameraOrigin", cameraOrigin);
        our_shader.set_vec3("cameraLookAt", cameraLookAt);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);

        glfwPollEvents();
        handleInputEvents(window);
    }

    glfwTerminate();
    return 0;
}
