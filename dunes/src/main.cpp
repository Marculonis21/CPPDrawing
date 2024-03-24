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

const uint screen_width = 1024;
const uint screen_height = 1024;
 
int num_frames{ 0 };
float last_time{ 0.0f };
 
// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
static std::vector<GLfloat> g_vertex_buffer_data = {};
static std::vector<GLuint> g_index_buffer_data = { };

// Two UV coordinatesfor each vertex. They were created with Blender.
static std::vector<GLfloat> g_uv_buffer_data = {};

void genQuadPlane(int xSize, int ySize, float step)
{
    const float max_x = step*xSize+step;
    const float max_y = step*ySize+step;

    for (int y = 0; y < ySize; y++) {
        for (int x = 0; x < xSize; ++x) {
            g_vertex_buffer_data.push_back(step*x);
            g_vertex_buffer_data.push_back(0);
            g_vertex_buffer_data.push_back(step*y);

            g_uv_buffer_data.push_back((step*x+0)/max_x);
            g_uv_buffer_data.push_back((step*y+step)/max_y);
        }
    }

    // TRIANGLES
    /* for (int y = 0; y < ySize-1; y++) { */
    /*     for (int x = 0; x < xSize-1; ++x) { */
    /*         g_index_buffer_data.push_back(y*xSize + xSize+x); */
    /*         g_index_buffer_data.push_back(y*xSize + x+1); */
    /*         g_index_buffer_data.push_back(y*xSize + x); */

    /*         g_index_buffer_data.push_back(y*xSize + xSize+x); */
    /*         g_index_buffer_data.push_back(y*xSize + xSize+x+1); */
    /*         g_index_buffer_data.push_back(y*xSize + x+1); */
    /*     } */
    /* } */

    // PATCHES
    for (int y = 0; y < ySize-1; y++) {
        for (int x = 0; x < xSize-1; ++x) {
            g_index_buffer_data.push_back((y+1)*xSize + x);
            g_index_buffer_data.push_back((y+1)*xSize + x+1);
            g_index_buffer_data.push_back(y*xSize     + x+1);
            g_index_buffer_data.push_back(y*xSize     + x);
        }
    }
}

inline void perlinToTexture(int x_quad_count, int y_quad_count, float scale, Texture2D &heightTexture, Texture2D &albedoTexture,
                            float lowf_freq=32.f, float midf_freq=8.f, float highf_freq=2.f,
                            float lowf_stre=1.f,  float midf_stre=0.2f,float highf_stre=0.05f,
                            float sandLevel=0.11, float grassLevel=0.6)
{
    GLubyte data[y_quad_count][x_quad_count][4] = {};
    GLubyte colorData[y_quad_count][x_quad_count][4] = {};
    for (size_t y = 0; y < y_quad_count; ++y) 
    {
        for (size_t x = 0; x < x_quad_count; ++x) 
        {
            float _x = x*scale;
            float _y = y*scale;
            float lowf = glm::perlin(glm::vec2(_x/lowf_freq,_y/lowf_freq));
            lowf = glm::pow(((lowf + 1) / 2.0f)*lowf_stre, 3.0f);

            float highf = glm::perlin(glm::vec2(_x/midf_freq,_y/midf_freq));
            highf = glm::pow(((highf + 1) / 2.0f)*midf_stre, 3.0f);

            float vhighf = glm::perlin(glm::vec2(_x/highf_freq,_y/highf_freq));
            vhighf = ((vhighf + 1) / 2.0f)*highf_stre;
            float value = lowf+highf+vhighf;

            value = glm::clamp(value-0.1f, 0.0f, 1.0f);

            data[y][x][0] = (GLubyte)(value*255);
            data[y][x][1] = (GLubyte)(value*255);
            data[y][x][2] = (GLubyte)(value*255);
            data[y][x][3] = (GLubyte)(value*255);

            if (value == 0.0f) {
                colorData[y][x][0] = (GLubyte)0;
                colorData[y][x][1] = (GLubyte)0;
                colorData[y][x][2] = (GLubyte)150;
            }
            else if (value < sandLevel) {
                colorData[y][x][0] = (GLubyte)255;
                colorData[y][x][1] = (GLubyte)250;
                colorData[y][x][2] = (GLubyte)212;
            }
            else if (value < grassLevel) {
                colorData[y][x][0] = (GLubyte)112;
                colorData[y][x][1] = (GLubyte)192;
                colorData[y][x][2] = (GLubyte)72;
            }
            else {
                colorData[y][x][0] = (GLubyte)85;
                colorData[y][x][1] = (GLubyte)89;
                colorData[y][x][2] = (GLubyte)92;
            }

            colorData[y][x][3] = (GLubyte)(255);
        }
    }

    heightTexture.AddData(GL_RGBA, GL_UNSIGNED_BYTE, data);
    albedoTexture.AddData(GL_RGBA, GL_UNSIGNED_BYTE, colorData);
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

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, screen_width/2, screen_height/2);

	// Dark blue background
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
 
    glViewport(0, 0, screen_width, screen_height);

	// Create and compile our GLSL program from the shaders
	/* Shader showingShader = Shader("assets/shaders/TransformVertexShader.glsl", "assets/shaders/TextureFragmentShader.glsl"); */
	Shader mainShader = Shader();

    mainShader.addShader("assets/shaders/TransformVertexShader.glsl", GL_VERTEX_SHADER);
    mainShader.addShader("assets/shaders/TextureFragmentShader.glsl", GL_FRAGMENT_SHADER);
    /* mainShader.addShader("assets/shaders/tesc.glsl", GL_TESS_CONTROL_SHADER); */
    /* mainShader.addShader("assets/shaders/tese.glsl", GL_TESS_EVALUATION_SHADER); */
    mainShader.linkProgram();

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(mainShader.program_ID, "MVP");

	/* // Load the texture */
	/* GLuint Texture = loadDDS("assets/uvtemplate.DDS"); */
	
    /* const int x_quad_count = 1000; */
    /* const int y_quad_count = 1000; */
    /* const float adj_step = 0.01; */
    /* const float perlin_adj = 1; */
    const int x_quad_count = 50;
    const int y_quad_count = 50;
    const float adj_step = 0.2;
    const float perlin_adj = 20;

    genQuadPlane(x_quad_count, y_quad_count,adj_step);
    std::cout << g_vertex_buffer_data.size() << std::endl;

    float lowf_freq = 200;
    float lowf_stre = 1.05;
    float midf_freq = 40;
    float midf_stre = 0.5;
    float highf_freq = 4;
    float highf_stre = -0.01;

    float sandLevel = 0.03;
    float grassLevel= 0.55;

    Texture2D perlinTexture(x_quad_count,y_quad_count,0, GL_RGBA32F);
    Texture2D albedoTexture(x_quad_count,y_quad_count,1, GL_RGBA32F);

    perlinToTexture(x_quad_count, y_quad_count, perlin_adj, perlinTexture, albedoTexture,
                    lowf_freq,midf_freq,highf_freq,
                    lowf_stre,midf_stre,highf_stre, sandLevel, grassLevel);


    GLuint arrayObj;
    glCreateVertexArrays(1, &arrayObj);
	glBindVertexArray(arrayObj);

	GLuint vertexBuffer;
    glCreateBuffers(1, &vertexBuffer);
    glNamedBufferStorage(vertexBuffer, sizeof(GL_FLOAT)*g_vertex_buffer_data.size(), g_vertex_buffer_data.data(), GL_MAP_READ_BIT);

	GLuint uvbuffer;
    glCreateBuffers(1, &uvbuffer);
    glNamedBufferStorage(uvbuffer, sizeof(GL_FLOAT)*g_uv_buffer_data.size(), g_uv_buffer_data.data(), GL_MAP_READ_BIT);

	GLuint indexBuffer;
    glCreateBuffers(1, &indexBuffer);
    glNamedBufferStorage(indexBuffer, sizeof(GL_UNSIGNED_INT)*g_index_buffer_data.size(), g_index_buffer_data.data(), GL_MAP_READ_BIT);
    glVertexArrayElementBuffer(arrayObj, indexBuffer);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(
        0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    bool wireframe = false;
    bool recenterMouse = false;

    glm::mat4 MVP;

    double lastSplit = 0;
    std::string fpsText = "";
    double lastTime = glfwGetTime();
    int nbFrames = 0;

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
        ImGui::Text("Rendering");
        ImGui::Text("%s", fpsText.data());
        ImGui::Checkbox("Wireframe", &wireframe);
        ImGui::Text("PerlinNoise");
        ImGui::PushItemWidth(ImGui::CalcItemWidth()/2);
        ImGui::Text("highf");
        ImGui::DragFloat("##highf_freq", &highf_freq, 0.01, 0.01);
        ImGui::SameLine();
        ImGui::DragFloat("##highf_stre", &highf_stre, 0.01, 0.01);
        ImGui::PopItemWidth();

        ImGui::PushItemWidth(ImGui::CalcItemWidth()/2);
        ImGui::Text("midf");
        ImGui::DragFloat("##midf_freq", &midf_freq, 0.01, 0.01);
        ImGui::SameLine();
        ImGui::DragFloat("##midf_stre", &midf_stre, 0.01, 0.01);
        ImGui::PopItemWidth();

        ImGui::PushItemWidth(ImGui::CalcItemWidth()/2);
        ImGui::Text("lowf");
        ImGui::DragFloat("##lowf_freq", &lowf_freq, 0.01, 0.01);
        ImGui::SameLine();
        ImGui::DragFloat("##lowf_stre", &lowf_stre, 0.01, 0.01);
        ImGui::PopItemWidth();

        ImGui::Text("Levels");
        ImGui::PushItemWidth(ImGui::CalcItemWidth()/2);
        ImGui::Text("Sand");
        ImGui::DragFloat("##sand", &sandLevel, 0.01, 0);
        ImGui::Text("Grass");
        ImGui::DragFloat("##grass", &grassLevel, 0.01, 0);
        ImGui::PopItemWidth();

        ImGui::End();

        if(glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS)
        {
            recenterMouse = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            // Hide the mouse and enable unlimited mouvement
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }


        if (wireframe) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        }
        else {
            glPolygonMode( GL_FRONT_AND_BACK,GL_FILL );
        }

        if(glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) wireframe = false;
        if(glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) wireframe = true;

		// Use our shader
        mainShader.useShader();

		// Compute the MVP matrix from keyboard and mouse input
        if(glfwGetKey(window, GLFW_KEY_SPACE ) != GLFW_PRESS)
        {
            computeMatricesFromInputs(window, screen_width, screen_height, recenterMouse);
            glm::mat4 ProjectionMatrix = getProjectionMatrix();
            glm::mat4 ViewMatrix = getViewMatrix();
            glm::mat4 ModelMatrix = glm::mat4(1.0);
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        }
        else
        {
            perlinToTexture(x_quad_count, y_quad_count, perlin_adj, perlinTexture, albedoTexture,
                            lowf_freq,midf_freq,highf_freq,
                            lowf_stre,midf_stre,highf_stre, sandLevel, grassLevel);
        }

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Bind our texture in Texture Unit 0
        
        mainShader.set_int("heightMapSampler",0);
        mainShader.set_int("albedoSampler",1);

        perlinTexture.Activate();
        albedoTexture.Activate();

		// Draw the triangle !
		/* glDrawArrays(GL_TRIANGLES, 0, x_quad_count*y_quad_count*2*3); // 12*3 indices starting at 0 -> 12 triangles */
		/* glDrawArrays(GL_POINTS, 0, x_quad_count*y_quad_count*2*3); // 12*3 indices starting at 0 -> 12 triangles */
		/* glDrawElements(GL_TRIANGLES, g_index_buffer_data.size(), GL_UNSIGNED_INT, 0); */ 
		glDrawElements(GL_PATCHES, g_index_buffer_data.size(), GL_UNSIGNED_INT, 0); 

		/* glDisableVertexAttribArray(0); */
		/* glDisableVertexAttribArray(1); */

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteVertexArrays(1, &arrayObj);
 
    glfwTerminate();
    return 0;
}
