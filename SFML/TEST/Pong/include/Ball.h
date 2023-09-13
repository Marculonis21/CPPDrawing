#pragma once
#include <SFML/Graphics.hpp>

class Ball {
private:
    sf::Vector2f        m_Position;    // X and Y position
    sf::RectangleShape  m_Shape;       // Sprite

    float m_Speed;          // Pixels per second
    float m_DirectionX;     // Horizontal velocity
    float m_DirectionY;     // Vertical velocity

    static constexpr float WIDTH{ 10.f };

public:
    Ball(const float startX, const float startY);
    ~Ball();
    
    /* Accessors */
    sf::FloatRect       getPosition() const;
    sf::RectangleShape  getShape() const;
    float               getVelocityY() const;

    void setPositionX(const float xPos);
    void setPositionY(const float yPos);

    /* Collision */
    void reboundSides();        // Side of screen collision
    void reboundBatOrTop();     // Bat or top of screen collision
    void reboundBottom();       // Bottom of screen collision

    /* Update */
    void update(sf::Time& dt);
};
