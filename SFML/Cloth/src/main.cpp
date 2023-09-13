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

#include "cloth.hpp"

int main(int argc, char* argv[]) {
    
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML Cloth");
    /* sf::View view = window.getDefaultView(); */
    /* view.zoom(0.8f); */
    /* window.setView(view); */

    bool RHOLD = false;
    bool LHOLD = false;
    float mouseRadius = 40;
    sf::Vector2i prevMousePos;

    sf::CircleShape cursor{mouseRadius,8};
    cursor.setOrigin(mouseRadius,mouseRadius);
    cursor.setFillColor(sf::Color::Transparent);
    cursor.setOutlineThickness(2);
    cursor.setOutlineColor(sf::Color::Green);

    Cloth cloth{150, 110, 4, sf::Vector2f{100,100}};

    sf::Clock clock;
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

            if (event.type == sf::Event::MouseButtonPressed) 
            {
                if (event.mouseButton.button == sf::Mouse::Right) 
                {
                    RHOLD = true;
                }
                if (event.mouseButton.button == sf::Mouse::Left) 
                {
                    LHOLD = true;
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) 
            {
                if (event.mouseButton.button == sf::Mouse::Right) 
                {
                    RHOLD = false;
                }
                if (event.mouseButton.button == sf::Mouse::Left) 
                {
                    LHOLD = false;
                }
            }

            if (event.type == sf::Event::MouseWheelScrolled) 
            {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) 
                {
                    mouseRadius = std::max(2.0f, mouseRadius + event.mouseWheelScroll.delta*2);
                }
            }
        }

        // LOGIC
        elapsed += clock.restart();
        while (elapsed >= update_ms) 
        {
            cloth.Update(update_ms);

            elapsed -= update_ms;
        }

        cloth.MouseEvents((sf::Vector2f)sf::Mouse::getPosition(window), (sf::Vector2f)prevMousePos, mouseRadius, LHOLD, RHOLD);

        window.clear(sf::Color::Black);
        window.draw(cloth);

        if (RHOLD) cursor.setOutlineColor(sf::Color::Red);
        else       cursor.setOutlineColor(sf::Color::Green);

        if (LHOLD)       cursor.setOutlineColor(sf::Color::Blue);
        else if (!RHOLD) cursor.setOutlineColor(sf::Color::Green);

        auto pos = sf::Mouse::getPosition(window);
        prevMousePos = pos;

        cursor.setPosition(pos.x, pos.y);
        cursor.setRadius(mouseRadius);
        cursor.setOrigin(mouseRadius, mouseRadius);
        window.draw(cursor);

        window.display();
    }
    
    return 0;
}
