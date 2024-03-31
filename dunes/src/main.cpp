#include <cmath>
#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <random>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/noise.hpp>

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "texture.hpp"
#include "texture2D.hpp"
#include "mesh.hpp"

extern glm::vec3 position;

const uint screen_width = 1024;
const uint screen_height = 1024;
 
int num_frames{ 0 };
float last_time{ 0.0f };


inline void perlinToTexture(int x_quad_count, int y_quad_count, float scale, Texture2D &heightTexture, Texture2D &albedoTexture,
                            float sandLevel=0.11, float grassLevel=0.6)
{
    std::cout << "data created" << std::endl;
    std::vector<GLubyte> data;
    std::vector<GLubyte> colorData;

    for (size_t y = 0; y < y_quad_count; ++y) 
    {
        for (size_t x = 0; x < x_quad_count; ++x) 
        {
            float _x = x*scale;
            float _y = y*scale;

            float value = 0;

            const int octaves = 5;
            float amplitude = 1;
            float frequency = 200;
            float totalAmplitude = 0;
            for (float i = 0; i < octaves; ++i)  {
                value += ((glm::perlin(glm::vec2(_x/frequency, _y/frequency))+1)/2)*amplitude;

                totalAmplitude += amplitude;
                amplitude *= 0.5;
                frequency /= 2.0;
            }

            value = glm::pow(value, 1.5);
            value = value/glm::pow(totalAmplitude,1.5);

            float waterLevel = 0.4f;

            data.push_back((GLubyte)(value*255));
            data.push_back((GLubyte)(value*255));
            data.push_back((GLubyte)(value*255));
            data.push_back((GLubyte)(value*255));

            value -= waterLevel;

            /* if (value  0.0f) { */
            /*     colorData.push_back((GLubyte)0); */
            /*     colorData.push_back((GLubyte)0); */
            /*     colorData.push_back((GLubyte)150); */
            /* } */
            if (value < sandLevel) {
                colorData.push_back((GLubyte)255);
                colorData.push_back((GLubyte)250);
                colorData.push_back((GLubyte)212);
            }
            else if (value < grassLevel-waterLevel) {
                colorData.push_back((GLubyte)112);
                colorData.push_back((GLubyte)192);
                colorData.push_back((GLubyte)72);
            }
            else {
                colorData.push_back((GLubyte)85);
                colorData.push_back((GLubyte)89);
                colorData.push_back((GLubyte)92);
            }

            colorData.push_back((GLubyte)(255));
        }
    }

    heightTexture.AddData(GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    albedoTexture.AddData(GL_RGBA, GL_UNSIGNED_BYTE, colorData.data());
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 
    GLFWwindow * window = glfwCreateWindow(screen_width, screen_height, "DunesGLFW", NULL, NULL);
 
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window!\n";
        glfwTerminate();
        return -1;
    }
 
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSYNC
 
    if (glewInit())
    {
        std::cout << "Failed initializing GLEW\n";
    }

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwPollEvents();
    glfwSetCursorPos(window, screen_width/2, screen_height/2);

	// Dark blue background
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
    glViewport(0, 0, screen_width, screen_height);

    glEnable( GL_BLEND );  
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	Shader mainShader;
    mainShader.addShader("assets/shaders/TransformVertexShader.glsl", GL_VERTEX_SHADER);
    mainShader.addShader("assets/shaders/TextureFragmentShader.glsl", GL_FRAGMENT_SHADER);
    mainShader.addShader("assets/shaders/tesc.glsl", GL_TESS_CONTROL_SHADER);
    mainShader.addShader("assets/shaders/tese.glsl", GL_TESS_EVALUATION_SHADER);
    mainShader.linkProgram();

	Shader seaShader;
    seaShader.addShader("assets/shaders/seaVertexShader.glsl", GL_VERTEX_SHADER);
    seaShader.addShader("assets/shaders/seaFragmentShader.glsl", GL_FRAGMENT_SHADER);
    seaShader.linkProgram();

    // Tesselation
    glPatchParameteri(GL_PATCH_VERTICES, 4);

    Mesh  terrain(10, 1, true);
    Mesh seaLevel(100, 0.1, false);

    float sandLevel  = 0.03;
    float grassLevel = 0.55;
    Texture2D perlinTexture(1000,1000,0, GL_RGBA32F);
    Texture2D albedoTexture(1000,1000,1, GL_RGBA32F);

    perlinToTexture(1000,1000, 1, perlinTexture, albedoTexture, sandLevel, grassLevel);

    std::cout << "perlin done" << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    bool wireframe = false;
    bool showMouse = false;

    glm::mat4 MVP;

    double lastSplit = 0;
    std::string fpsText = "";
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    float _sun[3];
    glm::vec3 sunPosition(-50,100,-50);
    _sun[0] = sunPosition.x;
    _sun[1] = sunPosition.y;
    _sun[2] = sunPosition.z;


    while (glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
    {
        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
            // printf and reset timer
            lastSplit = 1000.0/double(nbFrames);
            fpsText = "Speed: " + std::to_string(lastSplit);
            nbFrames = 0;
            lastTime += 1.0;
        }

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // do imgui drawing
        ImGui::Begin("ImGui win");
        ImGui::Text("X:%s, Y:%s, Z:%s", std::to_string(position.x).data(), std::to_string(position.y).data(), std::to_string(position.z).data());
        ImGui::Text("Rendering");
        ImGui::Text("%s", fpsText.data());
        ImGui::Checkbox("Wireframe", &wireframe);

        ImGui::Text("Levels");
        ImGui::PushItemWidth(ImGui::CalcItemWidth()/2);
        ImGui::Text("Sand");
        ImGui::DragFloat("##sand", &sandLevel, 0.01, 0);
        ImGui::Text("Grass");
        ImGui::DragFloat("##grass", &grassLevel, 0.01, 0);
        ImGui::PopItemWidth();

        ImGui::DragFloat3("sunPosition:", _sun);

        ImGui::End();

        if(glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS) { 
	        glfwSetCursorPos(window, screen_width/2, screen_height/2);
            showMouse = !showMouse;
        }
        
        if(glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) wireframe = false;
        if(glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) wireframe = true;

        glfwSetInputMode(window, GLFW_CURSOR, showMouse ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

        if(!showMouse)
        {
            computeMatricesFromInputs(window, screen_width, screen_height);
            glm::mat4 ProjectionMatrix = getProjectionMatrix();
            glm::mat4 ViewMatrix = getViewMatrix();
            glm::mat4 ModelMatrix = glm::mat4(1.0);
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        }

        perlinTexture.Activate();
        albedoTexture.Activate();

		// Use our shader
        mainShader.useShader();

        mainShader.set_mat4("MVP", MVP);
        mainShader.set_int("heightMapSampler",0);
        mainShader.set_int("albedoSampler",1);
        mainShader.set_vec3("cameraPos", position);
        sunPosition = glm::vec3(_sun[0], _sun[1], _sun[2]);
        mainShader.set_vec3("sunPosition", sunPosition);
        mainShader.set_float("seaLevel", 0.4);
        mainShader.set_vec3("origin", glm::vec3(0,0,0));

        terrain.activate();

	    glEnable(GL_CULL_FACE);
		glDrawElements(GL_PATCHES, terrain.indexCount, GL_UNSIGNED_INT, 0); 

        seaShader.useShader();
        seaShader.set_mat4("MVP", MVP);
        seaShader.set_int("heightMapSampler",0);
        seaShader.set_int("albedoSampler",1);
        sunPosition = glm::vec3(_sun[0], _sun[1], _sun[2]);
        seaShader.set_vec3("sunPosition", sunPosition);
        seaShader.set_float("seaLevel", 0.4);
        seaShader.set_vec3("cameraPos", position);
        seaShader.set_float("time", currentTime);

        seaLevel.activate();

        glDisable(GL_CULL_FACE);
		glDrawElements(GL_TRIANGLES, seaLevel.indexCount, GL_UNSIGNED_INT, 0); 

        /* seaLevel.activate(); */
		/* glDrawElements(GL_PATCHES, seaLevel.indexCount, GL_UNSIGNED_INT, 0); */ 

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
