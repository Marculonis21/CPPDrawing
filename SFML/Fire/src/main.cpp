#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
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

int main(int argc, char* argv[]) {
    
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML Cloth");

    ImGui::SFML::Init(window);

    bool LHOLD = false;
    bool clearColor = false;
    ParticleSim sim;

    sf::Clock clock;
    sf::Time elapsed = clock.restart();
    sf::Clock deltaClock;
    const sf::Time update_ms = sf::seconds(1.f / 60.f);
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

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)  LHOLD = true;
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) LHOLD = false;
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        if (LHOLD) 
        {
            sim.MouseEvent((sf::Vector2f)sf::Mouse::getPosition(window));
        }

        auto updateTimerStart = std::chrono::steady_clock::now();
        // LOGIC
        auto currentTime = clock.restart();

        elapsed += currentTime;
        while (elapsed >= update_ms) 
        {
            sim.Update(update_ms);

            elapsed -= update_ms;
        }
        auto updateTimerEnd = std::chrono::steady_clock::now();


        if (clearColor) window.clear(sf::Color::Black);
        else window.clear(sf::Color{55,55,55});

        auto simDrawStart = std::chrono::steady_clock::now();
        window.draw(sim);
        auto simDrawEnd = std::chrono::steady_clock::now();

        auto updateTimer = std::chrono::duration_cast<std::chrono::microseconds>(updateTimerEnd - updateTimerStart).count();
        auto drawTimer = std::chrono::duration_cast<std::chrono::microseconds>(simDrawEnd - simDrawStart).count();
        
        ImGui::Begin("Timing");
        ImGui::Text("Update timer: %ld", updateTimer);
        ImGui::Text("Draw timer: %ld", drawTimer);
        ImGui::End();

        ImGui::SFML::Render(window);

        window.display();
    }
    ImGui::SFML::Shutdown();
    window.close();
    
    return 0;
}
