#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <glm/glm.hpp>

void computeMatricesFromInputs(GLFWwindow* window, int screen_width, int screen_height);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

#endif
