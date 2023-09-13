#include "config.h"
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include <SFML/Graphics.hpp>

int main(int argc, char* argv[]) {
    
    // Output project version
    std::cout << "Version: " << PROJECT_VERSION_MAJOR << "."
         << PROJECT_VERSION_MINOR << std::endl;
	
    // Create a window
    sf::Window window(sf::VideoMode(800, 600), "SFML Boilerplate");

    // Handle closing the window
    while(window.isOpen()) {
        sf::Event Event;
        while(window.pollEvent(Event)) {
            if (Event.type == sf::Event::Closed)
                window.close();
            if (Event.type == sf::Event::KeyPressed) {
                if (Event.key.code == sf::Keyboard::Escape)
                    window.close();
            }
        }
        window.display();
    }
    
    return 0;
}
