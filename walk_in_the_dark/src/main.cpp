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
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
    };

    glm::vec2 pos;
    glm::vec2 lookDir;

    Player(glm::vec2 startPos, glm::vec2 startLookDir)
    {
        this->pos = startPos;
        this->lookDir = glm::normalize(startLookDir);
        this->rightDir = glm::cross(glm::vec3(lookDir.x, lookDir.y, 0), glm::vec3(0,0,1));
    }

    void move(PlayerDir moveDir)
    {
        switch (moveDir) 
        {
            case PlayerDir::FORWARD:
                pos += lookDir*speed;
                break;
            case PlayerDir::BACKWARD:
                pos += -lookDir*speed;
                break;
            case PlayerDir::LEFT:
                pos += -rightDir*speed;
                break;
            case PlayerDir::RIGHT:
                pos += rightDir*speed;
                break;
        }
    }

private:
    glm::vec2 rightDir;
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

    GLuint screenTex;
    glCreateTextures(GL_TEXTURE_2D, 1, &screenTex);
    glTextureParameteri(screenTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(screenTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(screenTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(screenTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(screenTex, 1, GL_RGBA32F, screen_width, screen_height);
    glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    GLuint envMap;
    glCreateTextures(GL_TEXTURE_2D, 1, &envMap);    
    glTexParameteri(envMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(envMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(envMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(envMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureStorage2D(envMap, 1, GL_RGBA32F, screen_width, screen_height);
    glBindImageTexture(1, envMap, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

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

    glTextureSubImage2D(envMap, 0, 0, 0, screen_width, screen_height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    std::string root{ROOT_DIR};
    auto vertexShader = root+"shaders/vertexShader.glsl";
    auto fragmentShader = root+"shaders/fragmentShader.glsl";
    Shader rectShader(vertexShader.c_str(), fragmentShader.c_str());

    GLuint agentSSbo;
    glCreateBuffers(1, &agentSSbo);

    std::vector<soundAgent> agentVector;
    agentVector.resize(5000);
    glNamedBufferData(agentSSbo, 500000*sizeof(soundAgent), NULL, GL_DYNAMIC_COPY);

    auto computeShaderPath = root+"shaders/computeShader.glsl";
    Compute computeShader(computeShaderPath.c_str());

    rectShader.set_int("screen", 0);
    rectShader.set_int("map", 1);

    computeShader.set_int("screen", 0);
    computeShader.set_int("map", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTex);

    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, envMap);

    Player player{glm::vec2{150,150}, glm::vec2{0,-1}};
    uint blank = 0;
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

        glTextureSubImage2D(screenTex, 0, 0, 0, screen_width, screen_height, GL_RGBA, GL_UNSIGNED_BYTE, screenData);

        countFPS();

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
        {
            player.move(Player::PlayerDir::LEFT);
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
        {
            player.move(Player::PlayerDir::RIGHT);
        }
        else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
        {
            player.move(Player::PlayerDir::FORWARD);

        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
        {
            player.move(Player::PlayerDir::BACKWARD);
        }
        else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) 
        {
            spacePressed = true;
        }
        else if (spacePressed && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) 
        {
            spacePressed = false;

            for (size_t i = 0; i < 5000; ++i) 
            {
                float radius = 10;
                auto circlePart = glm::pi<float>()/2500;

                auto dirx = glm::cos(circlePart*i);
                auto diry = glm::sin(circlePart*i);

                agentVector[i] = soundAgent{glm::vec4(player.pos.x+dirx*radius, player.pos.y+diry*radius,0,0),
                                            glm::vec4{dirx, diry, 0, 0}};
            }

            glNamedBufferSubData(agentSSbo, soundCounter*5000*sizeof(soundAgent), 5000*sizeof(soundAgent), &agentVector[0]);

            soundCounter = (soundCounter + 1)%100;
            std::cout << soundCounter << std::endl;
        }


        // compute shader part

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, agentSSbo);
        computeShader.use_shader(500000,1,1);
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
