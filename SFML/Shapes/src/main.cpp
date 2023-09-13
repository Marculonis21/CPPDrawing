#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

#include "particle.hpp"

int main(int argc, char* argv[]) {
    
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML Shapes");
    window.setFramerateLimit(60);

    std::vector<RoundParticle> particles;
    particles.push_back(RoundParticle{800,100,sf::Color::Red,sf::Vector2f{100,100}, sf::Vector2f{250,100}});
    particles.push_back(RoundParticle{800,20, sf::Color::Green,sf::Vector2f{700,400}, sf::Vector2f{-200,150}});
    particles.push_back(RoundParticle{800,50, sf::Color::Blue,sf::Vector2f{250,500}, sf::Vector2f{123,-70}});

    sf::Clock clock;
    sf::Time elapsed = clock.restart();
    const sf::Time update_ms = sf::seconds(1.f / 60.f);
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
        }

        // LOGIC

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
        window.display();
    }
    
    return 0;
}
