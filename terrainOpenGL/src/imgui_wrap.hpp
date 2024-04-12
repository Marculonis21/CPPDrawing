#ifndef IMGUI_WRAP_H
#define IMGUI_WRAP_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

class ImguiWrap
{

public:
    ImguiWrap(GLFWwindow *window);
    virtual ~ImguiWrap() {};

    void FramePreparation(glm::vec3 &position, const std::string &fpsText, bool wireframe, float waterLevel, float sandLevel, float grassLevel, float *_sun, float frequency, int octaves);
    void Render();
    void Destroy();
};

#endif 
