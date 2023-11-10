#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include <SFML/Graphics.hpp>

#include "particle.hpp"

#include "imgui.h"
#include "imgui-SFML.h"

int main(int argc, char* argv[]) {
    
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML Shapes");
    window.setFramerateLimit(60);

    ImGui::SFML::Init(window);

    std::vector<RoundParticle> particles;
    particles.push_back(RoundParticle{800,100,sf::Color::Red,sf::Vector2f{100,100}, sf::Vector2f{250,100}});
    particles.push_back(RoundParticle{800,20, sf::Color::Green,sf::Vector2f{700,400}, sf::Vector2f{-200,150}});
    particles.push_back(RoundParticle{800,50, sf::Color::Blue,sf::Vector2f{250,500}, sf::Vector2f{123,-70}});

    float c1[3] = {1,0,0};
    float c2[3] = {0,1,0};
    float c3[3] = {0,0,1};

    sf::Clock clock;
    sf::Time elapsed = clock.restart();
    const sf::Time update_ms = sf::seconds(1.f / 60.f);
    sf::Clock deltaClock;
    while (window.isOpen()) 
    {
        // EVENTS
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if ((event.type == sf::Event::Closed) ||
                ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
                window.close();
                break;
            }
        }
        ImGui::SFML::Update(window,deltaClock.restart());

        ImGui::Begin("TEST TEST");
        ImGui::Text("Window Text!");
        ImGui::ColorEdit3("P1 color:, ", c1);
        ImGui::ColorEdit3("P2 color:, ", c2);
        ImGui::ColorEdit3("P3 color:, ", c3);
        ImGui::End();

        // LOGIC
        particles[0].shape.setFillColor(sf::Color((int)(c1[0]*255), 
                                                  (int)(c1[1]*255),
                                                  (int)(c1[2]*255)));
        particles[1].shape.setFillColor(sf::Color((int)(c2[0]*255), 
                                                  (int)(c2[1]*255),
                                                  (int)(c2[2]*255)));
        particles[2].shape.setFillColor(sf::Color((int)(c3[0]*255), 
                                                  (int)(c3[1]*255),
                                                  (int)(c3[2]*255)));

        elapsed += clock.restart();
        while (elapsed >= update_ms) 
        {
            for(auto && particle : particles)
            {
                particle.Update(update_ms);
            }

            elapsed -= update_ms;
        }

        window.clear(sf::Color::Black);
        for(auto && particle : particles)
        {
            window.draw(particle.shape);
        }

        ImGui::SFML::Render(window);
        window.display();
    }
    
    ImGui::SFML::Shutdown(window);
    return 0;
}
