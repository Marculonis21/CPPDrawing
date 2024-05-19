#include <algorithm>
#include <cmath>
#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <tbb/tbb_stddef.h>
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

#include "imgui/imgui_internal.h"
#include "imgui_wrap.hpp"
#include "noise.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "texture.hpp"
#include "texture2D.hpp"
#include "mesh.hpp"

extern glm::vec3 position;

const uint screen_width = 1920;
const uint screen_height = 1080;
 
int num_frames{ 0 };
float last_time{ 0.0f };

#define VSYNCON 1

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
    glfwSwapInterval(VSYNCON); 
 
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

    // create shaders
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
        
	Compute noiseGenerator("assets/shaders/noise.glsl");

    // Tesselation params
    glPatchParameteri(GL_PATCH_VERTICES, 4);

    // create meshes
    Mesh  terrainMesh(10, 1, true);
    Mesh seaLevelMesh(100, 0.1, false);

    // perlin and textures parameters
    float sandLevel  = 0.03;
    float grassLevel = 0.55;
    float waterLevel = 0.3;

    float perlinFrequency = 1000;
    float perlinOctaves = 1;

    const int textureSize = 1024;

    Texture2D albedoHeightTexture(textureSize,textureSize, 0, GL_RGBA32F);
    Texture2D normalTexture(textureSize,textureSize, 1, GL_RGBA32F);
    /* Texture2D computeTexture(1000,1000,1, GL_RGBA32F); */

    std::vector<GLubyte> t1Data(textureSize*textureSize*4, 0);
    std::vector<GLubyte> t2Data(textureSize*textureSize*4, 0);
    for (int i = 0; i < t1Data.size()/4; ++i) {
        t1Data[i*4+0] = (GLubyte)0;
        t1Data[i*4+1] = (GLubyte)0;
        t1Data[i*4+2] = (GLubyte)0;
        t1Data[i*4+3] = (GLubyte)255;
        t2Data[i*4+0] = (GLubyte)0;
        t2Data[i*4+1] = (GLubyte)0;
        t2Data[i*4+2] = (GLubyte)0;
        t2Data[i*4+3] = (GLubyte)255;
    }
    albedoHeightTexture.AddData(GL_RGBA, GL_UNSIGNED_BYTE, t1Data.data());
    normalTexture.AddData(GL_RGBA, GL_UNSIGNED_BYTE, t2Data.data());

    std::cout << "perlin done" << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    bool wireframe = false;
    bool showMouse = false;
    bool spacePressed = false;
    bool reloadWanted = true;

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
        // FPS COUNTER
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
            // printf and reset timer
            lastSplit = 1000.0/double(nbFrames);
            fpsText = "Speed: " + std::to_string(lastSplit);
            nbFrames = 0;
            lastTime += 1.0;
        }

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // do imgui STUFF
        ImGui::Begin("ImGui win");
        ImGui::Text("X:%s, Y:%s, Z:%s", std::to_string(position.x).data(), std::to_string(position.y).data(), std::to_string(position.z).data());
        ImGui::Text("Rendering");
        ImGui::Text("%s", fpsText.data());
        ImGui::Checkbox("Wireframe", &wireframe);

        ImGui::Text("Perlin");
        ImGui::Text("frequency");
        ImGui::DragFloat("##freq", &perlinFrequency);
        ImGui::Text("octaves");
        ImGui::DragFloat("##octaves", &perlinOctaves, 0.1,0,10);
        // TODO: enable terrain color level changes
        ImGui::Text("Levels");
        ImGui::Text("Water");
        ImGui::DragFloat("##waterLevel", &waterLevel, 0.01, 0);
        ImGui::Text("Sand");
        ImGui::DragFloat("##sand", &sandLevel, 0.01, 0);
        ImGui::Text("Grass");
        ImGui::DragFloat("##grass", &grassLevel, 0.01, 0);

        ImGui::DragFloat3("sunPosition:", _sun);
        ImGui::End();

        if(glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS) spacePressed = true;

        if(spacePressed && glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_RELEASE) {
            spacePressed = false;
	        glfwSetCursorPos(window, screen_width/2, screen_height/2);
            showMouse = !showMouse;
        }
        
        if(glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) wireframe = false;
        if(glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) wireframe = true;

        glfwSetInputMode(window, GLFW_CURSOR, showMouse ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

        if(!showMouse) {
            computeMatricesFromInputs(window, screen_width, screen_height);
            glm::mat4 ProjectionMatrix = getProjectionMatrix();
            glm::mat4 ViewMatrix = getViewMatrix();
            glm::mat4 ModelMatrix = glm::mat4(1.0);
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            reloadWanted = false;
        }
        else {
            reloadWanted = true;
        }

        albedoHeightTexture.Activate();
        normalTexture.Activate();
        if (reloadWanted) {
            noiseGenerator.useShader(textureSize/1,textureSize/1,1);
            noiseGenerator.set_int("albedoHeightSampler",0);
            noiseGenerator.set_int("normalSampler",   1);
            noiseGenerator.set_int("octaves", int(perlinOctaves));
            noiseGenerator.set_float("sFreq", perlinFrequency);

            noiseGenerator.set_float("waterLevel", waterLevel);
            noiseGenerator.set_float("sandLevel", sandLevel);
            noiseGenerator.set_float("grassLevel", grassLevel);

            noiseGenerator.wait();
            /* albedoHeightTexture.Data(); */
            /* normalTexture.Data(); */
        }

        // terrain generation
        mainShader.useShader();

        mainShader.set_mat4("MVP", MVP);
        mainShader.set_int("albedoHeightTexture",0);
        mainShader.set_int("normalSampler", 1);
        mainShader.set_vec3("cameraPos", position);
        mainShader.set_vec3("sunPosition", glm::vec3(_sun[0], _sun[1], _sun[2]));
        mainShader.set_float("waterLevel", waterLevel);
        mainShader.set_float("sandLevel", sandLevel);
        mainShader.set_float("grassLevel", grassLevel);
        mainShader.set_vec3("origin", glm::vec3(0,0,0));

        terrainMesh.activate();

	    glEnable(GL_CULL_FACE);
		glDrawElements(GL_PATCHES, terrainMesh.indexCount, GL_UNSIGNED_INT, 0); 

        // water generation
        seaShader.useShader();
        seaShader.set_mat4("MVP", MVP);
        seaShader.set_int("albedoHeightTexture",0);
        seaShader.set_int("normalSampler", 1);
        seaShader.set_vec3("sunPosition", glm::vec3(_sun[0], _sun[1], _sun[2]));
        seaShader.set_float("time", currentTime);
        seaShader.set_float("waterLevel", waterLevel);
        seaShader.set_float("sandLevel", sandLevel);
        seaShader.set_float("grassLevel", grassLevel);
        seaShader.set_vec3("cameraPos", position);

        seaLevelMesh.activate();
        glDisable(GL_CULL_FACE);
		glDrawElements(GL_TRIANGLES, seaLevelMesh.indexCount, GL_UNSIGNED_INT, 0); 

        /* imguiWrap.Render(); */
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers
		glfwSwapBuffers(window);

		glfwPollEvents();
    }

    /* imguiWrap.Destroy(); */
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
