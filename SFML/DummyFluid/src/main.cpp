#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
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

#include "fluid.hpp"

/* #include "utils.hpp" */

#include "helpers/RootDir.hpp"
#include "utils.hpp"
std::string root{ROOT_DIR};

int main(int argc, char* argv[]) 
{
    // Create a window
    const int windowSize = 1024;
    sf::RenderWindow window(sf::VideoMode(windowSize, windowSize), "SFML Fluid");

    sf::Text text;
    sf::Font font;
    font.loadFromFile(root+"Simpletown.ttf");
    text.setFont(font);

    text.setPosition(sf::Vector2f{10,10});
    text.setFillColor(sf::Color::Red);
    text.setCharacterSize(24);

    FluidRect fluidRect{0.000001f,0.0000001f};

    int _size = FluidRect::size;

    sf::VertexBuffer vBuffer(sf::Quads);
    vBuffer.create(_size*_size*4);

    sf::Vertex verticies[_size*_size*4];

    float s = windowSize/_size;
    int x, y;
    float x0, y0;
    for (int i = 0; i < _size*_size*4; i+=4)
    {
        x = (i/4)%_size;
        y = (i/4)/_size;

        x0 = x*s;
        y0 = y*s;

        verticies[i+0].position = sf::Vector2f{x0+s,y0+s};
        verticies[i+1].position = sf::Vector2f{x0+s,y0+0};
        verticies[i+2].position = sf::Vector2f{x0+0,y0+0};
        verticies[i+3].position = sf::Vector2f{x0+0,y0+s};

        verticies[i+0].color = sf::Color::Black;
        verticies[i+1].color = sf::Color::Black;
        verticies[i+2].color = sf::Color::Black;
        verticies[i+3].color = sf::Color::Black;
    }

    vBuffer.update(verticies);

    bool LHOLD = false;
    sf::Vector2i pMouse;

    sf::Clock drawTimer;
    sf::Clock simTimer;

    float drawTime;
    float simTime;

    sf::Clock clock;
    sf::Time elapsed = clock.restart();
    const sf::Time update_ms = sf::seconds(FluidRect::dt);
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
            }

            if (event.type == sf::Event::MouseButtonPressed) 
            {
                if (event.mouseButton.button == sf::Mouse::Left) 
                {
                    LHOLD = true;
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) 
            {

                if (event.mouseButton.button == sf::Mouse::Left) 
                {
                    LHOLD = false;
                }
            }
        }

        int size = windowSize/FluidRect::size;
        auto mouse = sf::Mouse::getPosition(window);
        if (LHOLD) 
        {
            /* std::cout << pos.x/size << ":" << pos.y/size << std::endl; */

            fluidRect.AddDye(mouse.x/size - 1, mouse.y/size - 1, 0.75f);
            fluidRect.AddDye(mouse.x/size - 0, mouse.y/size - 1, 0.75f);
            fluidRect.AddDye(mouse.x/size + 1, mouse.y/size - 1, 0.75f);

            fluidRect.AddDye(mouse.x/size - 1, mouse.y/size - 0, 0.75f);
            fluidRect.AddDye(mouse.x/size - 0, mouse.y/size - 0, 0.75f);
            fluidRect.AddDye(mouse.x/size + 1, mouse.y/size - 0, 0.75f);

            fluidRect.AddDye(mouse.x/size - 1, mouse.y/size + 1, 0.75f);
            fluidRect.AddDye(mouse.x/size - 0, mouse.y/size + 1, 0.75f);
            fluidRect.AddDye(mouse.x/size + 1, mouse.y/size + 1, 0.75f);
        }

        auto vel = (mouse - pMouse);
        fluidRect.AddVelocity(mouse.x/size - 0, mouse.y/size - 0, vel.x*0.5f, vel.y*0.5f);
        pMouse = mouse;

        // LOGIC
        auto currentTime = clock.restart();

        elapsed += currentTime;
        while (elapsed >= update_ms) 
        {
            simTimer.restart();
            fluidRect.Step();
            simTime = simTimer.getElapsedTime().asSeconds();

            elapsed -= update_ms;
        }

        drawTimer.restart();

        for (int i = 0; i < _size*_size*4; i+=4)
        {
            float dens = fluidRect.density[i/4];
            float lastDens = fluidRect.s[i/4];

            auto col = sf::Color{sf::Uint8(dens*255), 0, 0};

            verticies[i+0].color = col;
            verticies[i+1].color = col;
            verticies[i+2].color = col;
            verticies[i+3].color = col;
        }
        vBuffer.update(verticies);
        window.draw(vBuffer);
        drawTime = drawTimer.getElapsedTime().asSeconds();

        text.setString("DrawTime: " + std::to_string(drawTime) + "\nSimTime: "+ std::to_string(simTime));
        window.draw(text);

        window.display();
    }
    
    return 0;
}
