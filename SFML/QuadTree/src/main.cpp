#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

#include "quadtreeV2.hpp"

int main(int argc, char* argv[]) {
    
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML Shapes");
    window.setFramerateLimit(60);

    QuadTree<Point> quadTree{sf::Vector2f{400,400}, 400};

    sf::RectangleShape cursorRange;
    float cursorSize = 20;
    cursorRange.setOrigin(cursorSize,cursorSize);
    cursorRange.setSize(sf::Vector2f{cursorSize*2, cursorSize*2});

    cursorRange.setFillColor(sf::Color::Transparent);
    cursorRange.setOutlineThickness(1);
    cursorRange.setOutlineColor(sf::Color::Red);

    std::default_random_engine engine{};
    std::uniform_real_distribution<float> distribution{0,800};

    std::vector<Point> allPoints;
    for (int i = 0; i < 100; ++i) 
    {
        allPoints.push_back(Point{sf::Vector2f{distribution(engine), distribution(engine)}});
        quadTree.Insert(allPoints[i].pos, i);
    }

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

            /* if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)  LHOLD = true; */
            /* if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) LHOLD = false; */

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f pos{sf::Mouse::getPosition(window)};

                allPoints.push_back(Point{pos});
                itemID index = allPoints.size()-1;
                quadTree.Insert(allPoints[index].pos, index);
            }
        }


        // LOGIC

        /* elapsed += clock.restart(); */
        /* while (elapsed >= update_ms) */ 
        /* { */
        /* } */

        window.clear(sf::Color::Black);

        window.draw(quadTree);
        sf::Vector2f pos{sf::Mouse::getPosition(window)};
        cursorRange.setPosition(pos);

        window.draw(cursorRange);

        std::vector<itemID> foundItems = quadTree.Range(pos, 20);

        for(auto id : foundItems) allPoints[id].Event();

        for(auto && p : allPoints) window.draw(p);

        for(auto id : foundItems) allPoints[id].Reset();

        window.display();
    }
    
    return 0;
}
