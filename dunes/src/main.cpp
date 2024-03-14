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
static std::vector<GLfloat> g_vertex_buffer_data = { 
    /* 0, 1, 0, */
    /* 0, 0, 0, */
    /* 1, 0, 0, */
    /* 1, 0, 0, */
    /* 1, 1, 0, */
    /* 0, 1, 0, */
    /* 1.0f,-1.0f, 1.0f, */
    /* -1.0f,-1.0f,-1.0f, */
    /* 1.0f,-1.0f,-1.0f, */
    /* 1.0f, 1.0f,-1.0f, */
    /* 1.0f,-1.0f,-1.0f, */
    /* -1.0f,-1.0f,-1.0f, */
    /* -1.0f,-1.0f,-1.0f, */
    /* -1.0f, 1.0f, 1.0f, */
    /* -1.0f, 1.0f,-1.0f, */
    /* 1.0f,-1.0f, 1.0f, */
    /* -1.0f,-1.0f, 1.0f, */
    /* -1.0f,-1.0f,-1.0f, */
    /* -1.0f, 1.0f, 1.0f, */
    /* -1.0f,-1.0f, 1.0f, */
    /* 1.0f,-1.0f, 1.0f, */
    /* 1.0f, 1.0f, 1.0f, */
    /* 1.0f,-1.0f,-1.0f, */
    /* 1.0f, 1.0f,-1.0f, */
    /* 1.0f,-1.0f,-1.0f, */
    /* 1.0f, 1.0f, 1.0f, */
    /* 1.0f,-1.0f, 1.0f, */
    /* 1.0f, 1.0f, 1.0f, */
    /* 1.0f, 1.0f,-1.0f, */
    /* -1.0f, 1.0f,-1.0f, */
    /* 1.0f, 1.0f, 1.0f, */
    /* -1.0f, 1.0f,-1.0f, */
    /* -1.0f, 1.0f, 1.0f, */
    /* 1.0f, 1.0f, 1.0f, */
    /* -1.0f, 1.0f, 1.0f, */
    /* 1.0f,-1.0f, 1.0f */
};

// Two UV coordinatesfor each vertex. They were created with Blender.
static std::vector<GLfloat> g_uv_buffer_data = { };
/* static const GLfloat g_uv_buffer_data[] = { */ 
/*     0.000059f, 0.000004f, */ 
/*     0.000103f, 0.336048f, */ 
/*     0.335973f, 0.335903f, */ 
/*     1.000023f, 0.000013f, */ 
/*     0.667979f, 0.335851f, */ 
/*     0.999958f, 0.336064f, */ 
/*     0.667979f, 0.335851f, */ 
/*     0.336024f, 0.671877f, */ 
/*     0.667969f, 0.671889f, */ 
/*     1.000023f, 0.000013f, */ 
/*     0.668104f, 0.000013f, */ 
/*     0.667979f, 0.335851f, */ 
/*     0.000059f, 0.000004f, */ 
/*     0.335973f, 0.335903f, */ 
/*     0.336098f, 0.000071f, */ 
/*     0.667979f, 0.335851f, */ 
/*     0.335973f, 0.335903f, */ 
/*     0.336024f, 0.671877f, */ 
/*     1.000004f, 0.671847f, */ 
/*     0.999958f, 0.336064f, */ 
/*     0.667979f, 0.335851f, */ 
/*     0.668104f, 0.000013f, */ 
/*     0.335973f, 0.335903f, */ 
/*     0.667979f, 0.335851f, */ 
/*     0.335973f, 0.335903f, */ 
/*     0.668104f, 0.000013f, */ 
/*     0.336098f, 0.000071f, */ 
/*     0.000103f, 0.336048f, */ 
/*     0.000004f, 0.671870f, */ 
/*     0.336024f, 0.671877f, */ 
/*     0.000103f, 0.336048f, */ 
/*     0.336024f, 0.671877f, */ 
/*     0.335973f, 0.335903f, */ 
/*     0.667969f, 0.671889f, */ 
/*     1.000004f, 0.671847f, */ 
/*     0.667979f, 0.335851f */
/* }; */

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

void genQuadPlane(int xSize, int ySize, float step)
{
    const float max_x = step*xSize+step;
    const float max_y = step*ySize+step;

    for (int y = 0; y < ySize; y++) {
        for (int x = 0; x < xSize; ++x) {
            g_vertex_buffer_data.push_back(step*x+0);
            g_vertex_buffer_data.push_back(step*y+step);
            g_vertex_buffer_data.push_back(0);
            g_vertex_buffer_data.push_back(step*x+0);
            g_vertex_buffer_data.push_back(step*y+0);
            g_vertex_buffer_data.push_back(0);
            g_vertex_buffer_data.push_back(step*x+step);
            g_vertex_buffer_data.push_back(step*y+0);
            g_vertex_buffer_data.push_back(0);

            g_vertex_buffer_data.push_back(step*x+step);
            g_vertex_buffer_data.push_back(step*y+0);
            g_vertex_buffer_data.push_back(0);
            g_vertex_buffer_data.push_back(step*x+step);
            g_vertex_buffer_data.push_back(step*y+step);
            g_vertex_buffer_data.push_back(0);
            g_vertex_buffer_data.push_back(step*x+0);
            g_vertex_buffer_data.push_back(step*y+step);
            g_vertex_buffer_data.push_back(0);


            g_uv_buffer_data.push_back((step*x+0)/max_x);
            g_uv_buffer_data.push_back((step*y+step)/max_y);

            g_uv_buffer_data.push_back((step*x+0)/max_x);
            g_uv_buffer_data.push_back((step*y+0)/max_y);

            g_uv_buffer_data.push_back((step*x+step)/max_x);
            g_uv_buffer_data.push_back((step*y+0)/max_y);

            g_uv_buffer_data.push_back((step*x+step)/max_x);
            g_uv_buffer_data.push_back((step*y+0)/max_y);

            g_uv_buffer_data.push_back((step*x+step)/max_x);
            g_uv_buffer_data.push_back((step*y+step)/max_y);

            g_uv_buffer_data.push_back((step*x+0)/max_x);
            g_uv_buffer_data.push_back((step*y+step)/max_y);
        }
    }
}

inline void perlinToTexture(int x_quad_count, int y_quad_count, Texture2D &texture,
                            float lowf_freq=32.f, float midf_freq=8.f, float highf_freq=2.f,
                            float lowf_stre=1.f,  float midf_stre=0.2f,float highf_stre=0.05f)
{
    GLubyte data[y_quad_count][x_quad_count][4] = {};
    for (size_t y = 0; y < y_quad_count; ++y) 
    {
        for (size_t x = 0; x < x_quad_count; ++x) 
        {
            float lowf = glm::perlin(glm::vec2(x/lowf_freq,y/lowf_freq));
            lowf = glm::pow(((lowf + 1) / 2.0f)*lowf_stre, 2.0f);

            float highf = glm::perlin(glm::vec2(x/midf_freq,y/midf_freq));
            highf = ((highf + 1) / 2.0f)*midf_stre;

            float vhighf = glm::perlin(glm::vec2(x/highf_freq,y/highf_freq));
            vhighf = ((vhighf + 1) / 2.0f)*highf_stre;
            float value = lowf+highf+vhighf;

            data[y][x][0] = (GLubyte)(value*255);
            data[y][x][1] = (GLubyte)(value*255);
            data[y][x][2] = (GLubyte)(value*255);
            data[y][x][3] = (GLubyte)(value*255);
        }
    }
    texture.AddData(GL_RGBA, GL_UNSIGNED_BYTE, data);
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
    glfwSwapInterval(1);
 
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
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
 
    glViewport(0, 0, screen_width, screen_height);

	// Create and compile our GLSL program from the shaders
	Shader showingShader = Shader("assets/shaders/TransformVertexShader.glsl", "assets/shaders/TextureFragmentShader.glsl");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(showingShader.program_ID, "MVP");

	/* // Load the texture */
	/* GLuint Texture = loadDDS("assets/uvtemplate.DDS"); */
	
    const int x_quad_count = 200;
    const int y_quad_count = 200;
    genQuadPlane(x_quad_count, y_quad_count,0.1);
    std::cout << g_vertex_buffer_data.size() << std::endl;


    float lowf_freq = 32;
    float lowf_stre = 1;
    float midf_freq = 8;
    float midf_stre = 0.2;
    float highf_freq = 2;
    float highf_stre = 0.05;

    Texture2D perlinTexture(x_quad_count,y_quad_count,0, GL_RGBA32F);

    perlinToTexture(x_quad_count, y_quad_count, perlinTexture,
                    lowf_freq,midf_freq,highf_freq,
                    lowf_stre,midf_stre,highf_stre);

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(showingShader.program_ID, "heightMapSampler");

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)*g_vertex_buffer_data.size(), g_vertex_buffer_data.data(), GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)*g_uv_buffer_data.size(), g_uv_buffer_data.data(), GL_STATIC_DRAW);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool wireframe = false;
    bool recenterMouse = false;

    glm::mat4 MVP;
    while (glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
    {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        perlinToTexture(x_quad_count, y_quad_count, perlinTexture,
                        lowf_freq,midf_freq,highf_freq,
                        lowf_stre,midf_stre,highf_stre);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // do imgui drawing
        ImGui::Begin("ImGui win");
        ImGui::Text("Rendering");
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
        showingShader.use_shader();

		// Compute the MVP matrix from keyboard and mouse input
        if(glfwGetKey(window, GLFW_KEY_SPACE ) != GLFW_PRESS)
        {
            computeMatricesFromInputs(window, screen_width, screen_height, recenterMouse);
            glm::mat4 ProjectionMatrix = getProjectionMatrix();
            glm::mat4 ViewMatrix = getViewMatrix();
            glm::mat4 ModelMatrix = glm::mat4(1.0);
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        }

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Bind our texture in Texture Unit 0
		/* glActiveTexture(GL_TEXTURE0); */
		/* glBindTexture(GL_TEXTURE_2D, Per); */
        perlinTexture.Activate();
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
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

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, x_quad_count*y_quad_count*2*3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


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
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(showingShader.program_ID);
	glDeleteTextures(1, &TextureID);
	glDeleteVertexArrays(1, &VertexArrayID);
 
    glfwTerminate();
    return 0;
}
