#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "particleSim.hpp"

#include "imgui.h"
#include "imgui-SFML.h"

#include <chrono>

const sf::Time update_ms = sf::seconds(1.f / 60.f);

int main(int argc, char* argv[]) {
    
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML Cloth");

    if (!ImGui::SFML::Init(window)) std::cout << "IMGUI problem" << std::endl;

    ParticleSim sim;

    bool clearColor = false;

    sf::Clock physicsClock;
    sf::Time physicsTime = physicsClock.restart();
    sf::Clock imguiClock;

    /* auto simDuration = std::chrono::microseconds(0); */
    /* auto drawDuration = std::chrono::microseconds(0); */
    long simDuration;
    long drawDuration;

    while (window.isOpen()) 
    {
        // EVENTS
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            if ((event.type == sf::Event::Closed) ||
                ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
                window.close();
                break;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) 
            {
                sim.AddParticle();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) 
            {
                sim.heatEnabled = !sim.heatEnabled;
                clearColor = !clearColor;
            }
        }

        ImGui::SFML::Update(window, imguiClock.restart());

        int updateCount = 0;
        auto updateTimerStart = std::chrono::steady_clock::now();
        // SIM
        physicsTime += physicsClock.restart();
        while (physicsTime >= update_ms) 
        {
            sim.Update(update_ms);
            updateCount++;

            physicsTime -= update_ms;
        }
        auto updateTimerEnd = std::chrono::steady_clock::now();


        if (clearColor) window.clear(sf::Color::Black);
        else window.clear(sf::Color{55,55,55});

        auto simDrawStart = std::chrono::steady_clock::now();
        window.draw(sim);
        auto simDrawEnd = std::chrono::steady_clock::now();

        if (std::chrono::duration_cast<std::chrono::microseconds>(updateTimerEnd - updateTimerStart).count() > 100) 
        {
            simDuration = std::chrono::duration_cast<std::chrono::microseconds>(updateTimerEnd - updateTimerStart).count()/updateCount;
        }
        drawDuration = std::chrono::duration_cast<std::chrono::microseconds>(simDrawEnd - simDrawStart).count();
        
        ImGui::Begin("Timing");
        ImGui::Text("Update timer: %ld", simDuration);
        ImGui::Text("Draw timer: %ld", drawDuration);
        ImGui::End();

        ImGui::SFML::Render(window);

        window.display();
    }
    ImGui::SFML::Shutdown();
    window.close();
    
    return 0;
}
