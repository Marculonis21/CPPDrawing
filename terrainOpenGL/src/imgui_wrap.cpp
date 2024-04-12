#include "imgui_wrap.hpp"
#include "imgui/imgui.h"

ImguiWrap::ImguiWrap(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void ImguiWrap::FramePreparation(glm::vec3 &position, const std::string &fpsText, bool wireframe, float waterLevel, float sandLevel, float grassLevel, float *_sun,
                                 float frequency, int octaves)
{
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
    ImGui::DragFloat("##freq", &frequency);
    ImGui::Text("octaves");
    ImGui::DragInt("##octaves", &octaves);
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
}

void ImguiWrap::Render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImguiWrap::Destroy()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
