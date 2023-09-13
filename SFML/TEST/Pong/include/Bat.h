#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class Bat {
private:
    sf::Vector2f m_Position;        // Bat x and y position
    sf::RectangleShape m_Shape;     // The bounding box sprite
    
    float m_Speed;                  // Movement in pixels per second
    bool  m_MovingRight;
    bool  m_MovingLeft;

    static const unsigned int WIDTH{ 250 };
    static const unsigned int HEIGHT{ 5 };

public:
    Bat(const float startX, const float startY);
    ~Bat();

    sf::FloatRect getPosition() const;
    sf::RectangleShape getShape() const;

    void moveLeft();
    void moveRight();
    void stopLeft();
    void stopRight();
    void update(sf::Time& dt);
};
