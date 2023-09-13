#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
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

#include "particleSim.hpp"

int main(int argc, char* argv[]) {
    
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML Cloth");

    bool LHOLD = false;

    ParticleSim sim;

    sf::Clock clock;
    float lastTime = 0;
    sf::Time elapsed = clock.restart();
    const sf::Time update_ms = sf::seconds(1.f / 240.f);
    while (window.isOpen()) 
    {
        // EVENTS
        sf::Event event;
        while (window.pollEvent(event)) {
            if ((event.type == sf::Event::Closed) ||
                ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
                window.close();
                break;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) 
            {
                sim.AddParticle();
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)  LHOLD = true;
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) LHOLD = false;
        }

        if (LHOLD) 
        {
            sim.MouseEvent((sf::Vector2f)sf::Mouse::getPosition(window));
        }

        // LOGIC
        auto currentTime = clock.restart();

        elapsed += currentTime;
        while (elapsed >= update_ms) 
        {
            sim.Update(update_ms);

            elapsed -= update_ms;
        }

        window.clear(sf::Color{55,55,55});

        window.draw(sim);

        window.display();

        std::cout << 1.0f/(currentTime.asSeconds() - lastTime) << std::endl;

        lastTime = currentTime.asSeconds();
    }
    
    return 0;
}
