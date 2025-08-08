#include <algorithm>
#include <cmath>
#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <random>
#include <stdlib.h>
#include <string>
#include <tbb/tbb_stddef.h>
#include <vector>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "controls.hpp"
#include "imgui/imgui_internal.h"
#include "imgui_wrap.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "texture2D.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

extern glm::vec3 position;

const uint screen_width = 1024;
const uint screen_height = 1024;

int num_frames{0};
float last_time{0.0f};

#define VSYNCON 0

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window =
        glfwCreateWindow(screen_width, screen_height, "DunesGLFW", NULL, NULL);

    if (window == nullptr) {
        std::cout << "Failed to create GLFW window!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(VSYNCON);

    if (glewInit()) {
        std::cout << "Failed initializing GLEW\n";
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwPollEvents();
    glfwSetCursorPos(window, screen_width / 2.0, screen_height / 2.0);

    // Dark blue background
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    /* glClearColor(0.5,0.7,1.0,1); */

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glViewport(0, 0, screen_width, screen_height);

    //glShadeModel(GL_SMOOTH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // create shaders
    Shader mainShader;
    mainShader.addShader("assets/shaders/TransformVertexShader.glsl",
                         GL_VERTEX_SHADER);
    mainShader.addShader("assets/shaders/TextureFragmentShader.glsl",
                         GL_FRAGMENT_SHADER);
    mainShader.addShader("assets/shaders/tesc.glsl", GL_TESS_CONTROL_SHADER);
    mainShader.addShader("assets/shaders/tese.glsl", GL_TESS_EVALUATION_SHADER);
    mainShader.linkProgram();

    Shader waterDrawShader;
    waterDrawShader.addShader("assets/shaders/water_vertex.glsl",
                              GL_VERTEX_SHADER);
    waterDrawShader.addShader("assets/shaders/water_fragment.glsl",
                              GL_FRAGMENT_SHADER);
    waterDrawShader.addShader("assets/shaders/water_tesc.glsl",
                              GL_TESS_CONTROL_SHADER);
    waterDrawShader.addShader("assets/shaders/water_tese.glsl",
                              GL_TESS_EVALUATION_SHADER);
    waterDrawShader.linkProgram();

    /* Shader seaShader; */
    /* seaShader.addShader("assets/shaders/seaVertexShader.glsl",
     * GL_VERTEX_SHADER); */
    /* seaShader.addShader("assets/shaders/seaFragmentShader.glsl",
     * GL_FRAGMENT_SHADER); */
    /* seaShader.linkProgram(); */

    Compute noiseGenerator("assets/shaders/noise.glsl");
    Compute terrainShader("assets/shaders/terrain.glsl");

    Compute erosionSource("assets/shaders/erosion_source.glsl");
    Compute erosionFlow("assets/shaders/erosion_flow.glsl");
    Compute erosionFlowVelocityField("assets/shaders/erosion_flow_velocity_field.glsl");
    Compute erosionDeposition("assets/shaders/erosion_deposition.glsl");
    Compute erosionTransport("assets/shaders/erosion_transport.glsl");
    Compute erosionEvaporation("assets/shaders/erosion_evaporation.glsl");

    // Tesselation params
    glPatchParameteri(GL_PATCH_VERTICES, 4);

    // create meshes
    Mesh terrainMesh(32, 0.25, true);
    //Mesh terrainMesh(8, 1, true);
    /* Mesh seaLevelMesh(100, 0.1, false); */
    Mesh waterMesh(32, 0.25, true);
    //Mesh waterMesh(32, 0.25, true);

    // perlin and textures parameters
    float sandLevel = 0.1;
    float grassLevel = 0.9;
    float waterLevel = 0.0;

    float perlinFrequency = 800;
    float perlinOctaves = 8;

    const int textureSize = 2048;
    const int waterTextureSize = 2048;
    const float tScalingF = (float)textureSize/waterTextureSize;

    const float erosionTimeStep = 0.0025;

    Texture2D albedoHeightTexture(textureSize, textureSize, 0, GL_RGBA32F);
    Texture2D normalTexture(textureSize, textureSize, 1, GL_RGBA32F);

    Texture2D waterTexture(waterTextureSize,waterTextureSize, 2, GL_RGBA32F);
    Texture2D waterFlowTexture(waterTextureSize,waterTextureSize, 3, GL_RGBA32F);
    Texture2D sedimentTexture(waterTextureSize,waterTextureSize, 4, GL_RGBA32F);

    // Load textures
    Texture2D rock_face_albedo("assets/textures/rock_face_03_1k/rock_face_03_diff_1k.png", 5, GL_RGB);
    Texture2D rock_face_arm("assets/textures/rock_face_03_1k/rock_face_03_arm_1k.png", 6, GL_RGB);
    Texture2D rock_face_normal("assets/textures/rock_face_03_1k/rock_face_03_nor_gl_1k.png", 7, GL_RGB);

    Texture2D terrain_albedo("assets/textures/rocky_terrain_02_1k/rocky_terrain_02_diff_1k.png", 8);
    Texture2D terrain_arm("assets/textures/rocky_terrain_02_1k/rocky_terrain_02_arm_1k.png", 9, GL_RGB);
    Texture2D terrain_normal("assets/textures/rocky_terrain_02_1k/rocky_terrain_02_nor_gl_1k.png", 10);

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

    bool enterPressed = false;
    bool addWater = false;
    bool reloadTerrain = false;

    glm::mat4 MVP;

    double lastSplit = 0;
    std::string fpsText = "";
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    float _sun[3];
    glm::vec3 sunPosition(1, 1, 1);
    _sun[0] = sunPosition.x;
    _sun[1] = sunPosition.y;
    _sun[2] = sunPosition.z;

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0) {
        // FPS COUNTER
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1
                                             // sec ago printf and reset timer
            lastSplit = 1000.0 / double(nbFrames);
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
        ImGui::Text("X:%s, Y:%s, Z:%s", std::to_string(position.x).data(),
                    std::to_string(position.y).data(),
                    std::to_string(position.z).data());
        ImGui::Text("Rendering");
        ImGui::Text("%s", fpsText.data());
        ImGui::Checkbox("Wireframe", &wireframe);

        ImGui::Text("Perlin");
        ImGui::Text("frequency");
        ImGui::DragFloat("##freq", &perlinFrequency);
        ImGui::Text("octaves");
        ImGui::DragFloat("##octaves", &perlinOctaves, 0.1, 0, 10);

        ImGui::Text("Levels");
        ImGui::Text("Water");
        ImGui::DragFloat("##waterLevel", &waterLevel, 0.01, 0);
        ImGui::Text("Sand");
        ImGui::DragFloat("##sand", &sandLevel, 0.01, 0);
        ImGui::Text("Grass");
        ImGui::DragFloat("##grass", &grassLevel, 0.01, 0);

        ImGui::DragFloat3("sunPosition:", _sun, 0.1, 0.0);
        ImGui::End();

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            spacePressed = true;

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            reloadTerrain = true;
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
            reloadTerrain = false;

        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
            addWater = true;
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) 
            addWater = false;

        if (spacePressed &&
            glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            spacePressed = false;
            glfwSetCursorPos(window, screen_width / 2.0, screen_height / 2.0);
            showMouse = !showMouse;
        }

        if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
            wireframe = false;
        if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
            wireframe = true;

        glfwSetInputMode(window, GLFW_CURSOR,
                         showMouse ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

        if (!showMouse) {
            computeMatricesFromInputs(window, screen_width, screen_height);
            glm::mat4 ProjectionMatrix = getProjectionMatrix();
            glm::mat4 ViewMatrix = getViewMatrix();
            glm::mat4 ModelMatrix = glm::mat4(1.0);
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        }

        albedoHeightTexture.Activate();
        normalTexture.Activate();
        waterTexture.Activate();
        waterFlowTexture.Activate();
        sedimentTexture.Activate();

        rock_face_albedo.Activate();
        rock_face_arm.Activate();
        rock_face_normal.Activate();

        terrain_albedo.Activate();
        terrain_arm.Activate();
        terrain_normal.Activate();

        if (reloadTerrain) {
            noiseGenerator.useShader(textureSize / 32, textureSize / 32, 1);
            noiseGenerator.set_int("albedoHeightSampler", 0);
            noiseGenerator.set_int("waterTextureSampler", 2);
            noiseGenerator.set_int("waterFlowSampler", 3);
            noiseGenerator.set_int("sedimentSampler", 4);
            noiseGenerator.set_int("octaves", int(perlinOctaves));
            noiseGenerator.set_float("sFreq", perlinFrequency);

            noiseGenerator.wait();
        }

        terrainShader.useShader(textureSize / 32, textureSize / 32, 1);
        terrainShader.set_int("tTextureSize", textureSize);
        terrainShader.set_int("albedoHeightSampler", 0);
        terrainShader.set_int("normalSampler", 1);
        terrainShader.set_float("waterLevel", waterLevel);
        terrainShader.set_float("sandLevel", sandLevel);
        terrainShader.set_float("grassLevel", grassLevel);
        terrainShader.set_vec3("sunPosition",
                               glm::vec3(_sun[0], _sun[1], _sun[2]));
        terrainShader.wait();

        if (addWater) {
            erosionSource.useShader(waterTextureSize/ 32, waterTextureSize/ 32, 1);
            erosionSource.set_int("waterTextureSampler", 2);
            erosionSource.set_float("timeStep", erosionTimeStep);
            erosionSource.wait();
        }
        else {
            /* https://inria.hal.science/inria-00402079/document */
            /* https://harald.ist.org/paste/erosion.pdf */
            // erosion process
            erosionFlow.useShader(waterTextureSize/ 32, waterTextureSize/ 32, 1);
            erosionFlow.set_int("tTextureSize", textureSize);
            erosionFlow.set_int("wTextureSize", waterTextureSize);
            erosionFlow.set_int("albedoHeightSampler", 0);
            erosionFlow.set_int("waterTextureSampler", 2);
            erosionFlow.set_int("waterFlowSampler", 3);
            erosionFlow.set_float("timeStep", erosionTimeStep);
            erosionFlow.wait();

            erosionFlowVelocityField.useShader(waterTextureSize/ 32, waterTextureSize/ 32, 1);
            erosionFlowVelocityField.set_int("wTextureSize", waterTextureSize);
            erosionFlowVelocityField.set_int("waterTextureSampler", 2);
            erosionFlowVelocityField.set_int("waterFlowSampler", 3);
            erosionFlowVelocityField.set_float("timeStep", erosionTimeStep);
            erosionFlowVelocityField.wait();

            erosionDeposition.useShader(waterTextureSize/ 32, waterTextureSize/ 32, 1);
            erosionDeposition.set_int("tTextureSize", textureSize);
            erosionDeposition.set_int("wTextureSize", waterTextureSize);
            erosionDeposition.set_float("tScalingF", tScalingF);
            erosionDeposition.set_int("albedoHeightSampler", 0);
            erosionDeposition.set_int("normalSampler", 1);
            erosionDeposition.set_int("waterTextureSampler", 2);
            erosionDeposition.set_int("waterFlowSampler", 3);
            erosionDeposition.set_int("sedimentSampler", 4);
            erosionDeposition.set_float("timeStep", erosionTimeStep);

            erosionDeposition.set_float("K_C", 1.0);
            erosionDeposition.set_float("ALPHA_MIN", 0.4);
            erosionDeposition.set_float("K_DMAX", 1.0);
            erosionDeposition.set_float("K_S", 0.05);
            erosionDeposition.set_float("K_D", 0.01);
            erosionDeposition.set_float("K_H", 0.01);
            erosionDeposition.set_float("K_H_MIN", 0.1);
            erosionDeposition.wait();

            erosionTransport.useShader(waterTextureSize/ 32, waterTextureSize/ 32, 1);
            erosionTransport.set_int("tTextureSize", textureSize);
            erosionTransport.set_int("wTextureSize", waterTextureSize);
            erosionTransport.set_int("waterTextureSampler", 2);
            erosionTransport.set_int("waterFlowSampler", 3);
            erosionTransport.set_int("sedimentSampler", 4);
            erosionTransport.set_float("timeStep", erosionTimeStep);
            erosionTransport.wait();

            erosionEvaporation.useShader(waterTextureSize/ 32, waterTextureSize/ 32, 1);
            erosionEvaporation.set_int("waterTextureSampler", 2);
            erosionEvaporation.set_float("timeStep", erosionTimeStep);
            erosionEvaporation.set_float("evaporationConst", 0.01);
            erosionEvaporation.wait();
        }

        // terrain generation
        mainShader.useShader();

        mainShader.set_int("tTextureSize", textureSize);
        mainShader.set_mat4("MVP", MVP);
        mainShader.set_int("albedoHeightSampler", 0);
        mainShader.set_int("normalSampler", 1);
        mainShader.set_int("waterTextureSampler", 2);
        mainShader.set_int("waterFlowSampler", 3);
        mainShader.set_int("sedimentSampler", 4);

        mainShader.set_int("rock_face_albedo_s", 5);
        mainShader.set_int("rock_face_arm_s", 6);
        mainShader.set_int("rock_face_normal_s", 7);

        mainShader.set_int("terrain_albedo_s", 8);
        mainShader.set_int("terrain_arm_s", 9);
        mainShader.set_int("terrain_normal_s", 10);


        mainShader.set_vec3("cameraPos", position);
        mainShader.set_vec3("sunDirection", glm::vec3(_sun[0], _sun[1], _sun[2]));
        mainShader.set_float("waterLevel", waterLevel);
        mainShader.set_float("sandLevel", sandLevel);
        mainShader.set_float("grassLevel", grassLevel);

        terrainMesh.activate();

        glEnable(GL_CULL_FACE);
        glDrawElements(GL_PATCHES, terrainMesh.indexCount, GL_UNSIGNED_INT, 0);

        waterDrawShader.useShader();
        /* waterDrawShader.set_int("tTextureSize", textureSize); */
        waterDrawShader.set_mat4("MVP", MVP);
        waterDrawShader.set_vec3("cameraPos", position);
        waterDrawShader.set_vec3("sunDirection", glm::vec3(_sun[0], _sun[1], _sun[2]));
        waterDrawShader.set_int("albedoHeightSampler", 0);
        waterDrawShader.set_int("waterTextureSampler", 2);
        waterDrawShader.set_int("waterFlowSampler", 3);
        waterDrawShader.set_int("sedimentSampler", 4);

        waterMesh.activate();
        glDrawElements(GL_PATCHES, waterMesh.indexCount, GL_UNSIGNED_INT, 0);

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
