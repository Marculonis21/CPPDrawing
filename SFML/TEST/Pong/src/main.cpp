#include "config.h"
#include "Bat.h"
#include "Ball.h"
#include <sstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

int main(int argc, char* argv[]) 
{
    // Create a video mode object
    sf::VideoMode vm(800.f, 600.f);

    // Create and open a window
    sf::RenderWindow window(vm, "Pong", sf::Style::Default);
    sf::View view(sf::FloatRect(0, 0, 800.f, 600.f));
    window.setView(view);

    int score{ 0 };
    int lives{ 3 };

    // Create a bat at the bottom center of the screen
    Bat bat(400.f, 600.f-20.f);
   
    // Create a ball
    Ball ball(400.f, 0.f);

    // Load font
    sf::Font font;
    std::string source_dir = RESOURCE_PATH; 

#ifndef NDEBUG
    // Output RESOURCE_PATH
    std::cout << "RESOURCE_PATH: " << source_dir << std::endl;
#endif

    if (!font.loadFromFile(source_dir + "sansation.ttf"))
        return EXIT_FAILURE;

    // Load texture
    sf::Texture texture;
    if (!texture.loadFromFile(source_dir + "background.png"))
        return EXIT_FAILURE;

    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setPosition(0.f, 0.f);
    unsigned int c = 212;
    sprite.setColor(sf::Color(c,c,c,255));

    // Load sound
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(source_dir + "bip.wav"))
        return EXIT_FAILURE;

    sf::Sound sound;
    sound.setBuffer(buffer);

    // Create a text object
    sf::Text hud;
    hud.setFont(font);
    hud.setCharacterSize(24);
    hud.setFillColor(sf::Color::White);
    hud.setPosition(10.f, 10.f);
    
    // Clock for timing everything
    sf::Clock clock;

    // Flag for updating the score
    bool updateScore{ false };

    while (window.isOpen())
    {
        /* Handle the player input
         *****************************************/
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if (event.type == sf::Event::KeyPressed)
            {
                // Escape
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                
                // Left arrow
                if (event.key.code == sf::Keyboard::Left)
                    bat.moveLeft();

                // Right arrow
                if (event.key.code == sf::Keyboard::Right)
                   bat.moveRight(); 
            }
            
            if (event.type == sf::Event::KeyReleased) 
            {
                // Left arrow
                if (event.key.code == sf::Keyboard::Left)
                    bat.stopLeft();

                // Right arrow
                if (event.key.code == sf::Keyboard::Right)
                   bat.stopRight();  
            }
        }

        /*
         * Update
         *****************************************/
        // Update delta time
        sf::Time dt = clock.restart();

        bat.update(dt);
        ball.update(dt);

        // Handle ball hiting the bottom
        if (ball.getPosition().top > view.getSize().y) 
        {
            ball.reboundBottom(); // Reverse ball Y velocity
            --lives;              // Remove a life
            
            if (lives < 1) {      // Check for zero lives
                score = 0;        // Reset the score
                lives = 3;        // Reest the lives
            }
        }

        // Handle ball hitting top
        if (ball.getPosition().top < 0.f) {
            sound.play();
            ball.setPositionY(0.f);
            ball.reboundBatOrTop();  // Reverse ball Y velocity
            updateScore = true;      // Set flag to increment score 
        }

        // Handle ball hitting sides
        if (ball.getPosition().left < 0) {
            sound.play();
            ball.setPositionX(0);
            ball.reboundSides();     // Reverse ball X velocity
        }

        if (ball.getPosition().left + ball.getPosition().width > view.getSize().x) {
            sound.play();
            ball.setPositionX(view.getSize().x - ball.getPosition().width);
            ball.reboundSides();
        }

        // Has the ball hit the bat?
        if (ball.getPosition().intersects(bat.getPosition())) {
            sound.play();
            ball.setPositionY(bat.getPosition().top - ball.getPosition().height);
            ball.reboundBatOrTop();   // Reverse ball Y velocity
            updateScore = true;
        }

        // Increment score
        if (updateScore) {
            ++score;
            updateScore = false;
        }

        // Update HUD
        std::stringstream ss;
        ss << "Score: " << score << "   Lives: " << lives;
        hud.setString(ss.str());

        /*
         * Draw
         ****************************************/
        window.clear();
        window.draw(sprite);
        window.draw(hud);
        window.draw(bat.getShape());
        window.draw(ball.getShape());
        window.display();
    }

    return EXIT_SUCCESS;
}
