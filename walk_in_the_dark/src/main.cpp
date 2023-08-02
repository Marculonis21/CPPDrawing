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
 
#include "helpers/RootDir.hpp"

#include "shader.hpp"
#include "texture.hpp"
 
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


struct Player
{
    enum class PlayerDir
    {
        UP,
        DOWN,
        LEFT,
        RIGHT,
    };

    glm::vec2 pos;

    Player(glm::vec2 startPos)
    {
        this->pos = startPos;
    }

    void move(PlayerDir moveDir)
    {
        switch (moveDir) 
        {
            case PlayerDir::UP:
                pos += glm::vec2(0, -1)*speed;
                break;
            case PlayerDir::DOWN:
                pos += glm::vec2(0, 1)*speed;
                break;
            case PlayerDir::LEFT:
                pos += glm::vec2(-1, 0)*speed;
                break;
            case PlayerDir::RIGHT:
                pos += glm::vec2(1, 0)*speed;
                break;
        }
    }

private:
    const float speed = 2;
};

struct soundAgent
{
    glm::vec4 position; 
    glm::vec4 dir;
    alignas(16) float lifeCycle=100;
};


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
    Texture2D screenTex{screen_width, screen_height, 0, GL_RGBA32F};
    Texture2D envTex{screen_width, screen_height, 1, GL_RGBA32F};

    std::string charMap[10] {"##########",
                             "#        #",
                             "# ###### #",
                             "#  #  #  #",
                             "# ## ##  #",
                             "# ##  ## #",
                             "#  ##    #",
                             "#  ##### #",
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

    envTex.AddData(GL_RGBA, GL_UNSIGNED_BYTE, data);

    std::string root{ROOT_DIR};
    auto vertexShader = root+"shaders/vertexShader.glsl";
    auto fragmentShader = root+"shaders/fragmentShader.glsl";
    Shader rectShader(vertexShader.c_str(), fragmentShader.c_str());

    GLuint agentSSbo;
    glCreateBuffers(1, &agentSSbo);

    std::vector<soundAgent> agentVector;
    const size_t TRACER_COUNT = 90;

    agentVector.resize(TRACER_COUNT);
    glNamedBufferData(agentSSbo, TRACER_COUNT*100*sizeof(soundAgent), NULL, GL_DYNAMIC_COPY);

    auto computeShaderPath = root+"shaders/computeShader.glsl";
    Compute computeShader(computeShaderPath.c_str());

    rectShader.set_int("screen", 0);
    rectShader.set_int("map", 1);

    computeShader.set_int("screen", 0);
    computeShader.set_int("map", 1);

    screenTex.Activate();
    envTex.Activate();

    Player player{glm::vec2{150,150}};
    uint soundCounter = 0;
    bool spacePressed = false;

    while (!glfwWindowShouldClose(window))
    {
        GLubyte screenData[screen_height][screen_width][4] = {};
        for (int i = 0; i < 360; ++i) 
        {
            float radius = 20;
            auto circlePart = glm::pi<float>()/180;
            auto posx = glm::cos(circlePart*i)*radius;
            auto posy = glm::sin(circlePart*i)*radius;

            glm::vec2 pos = player.pos + glm::vec2{posx, posy};
            screenData[int(pos.y)][int(pos.x)][0] = (GLubyte)255;
            screenData[int(pos.y)][int(pos.x)][1] = (GLubyte)255;
            screenData[int(pos.y)][int(pos.x)][2] = (GLubyte)255;
            screenData[int(pos.y)][int(pos.x)][3] = (GLubyte)255;
        }

        screenTex.AddData(GL_RGBA, GL_UNSIGNED_BYTE, screenData);

        countFPS();

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
        {
            player.move(Player::PlayerDir::LEFT);
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
        {
            player.move(Player::PlayerDir::RIGHT);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
        {
            player.move(Player::PlayerDir::UP);

        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
        {
            player.move(Player::PlayerDir::DOWN);
        }
        
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) 
        {
            spacePressed = true;
        }
        else if (spacePressed && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) 
        {
            spacePressed = false;

            for (size_t i = 0; i < TRACER_COUNT; ++i) 
            {
                float radius = 10;
                auto circlePart = glm::pi<float>()*2/TRACER_COUNT;

                auto dirx = glm::cos(circlePart*i);
                auto diry = glm::sin(circlePart*i);

                agentVector[i] = soundAgent{glm::vec4(player.pos.x+dirx*radius, player.pos.y+diry*radius,0,0),
                                            glm::vec4{dirx, diry, 0, 0}};
            }

            glNamedBufferSubData(agentSSbo, soundCounter*TRACER_COUNT*sizeof(soundAgent), TRACER_COUNT*sizeof(soundAgent), &agentVector[0]);

            soundCounter = (soundCounter + 1)%100;
            std::cout << soundCounter << std::endl;
        }

        // compute shader part

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, agentSSbo);
        computeShader.use_shader(TRACER_COUNT*100,1,1);
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
