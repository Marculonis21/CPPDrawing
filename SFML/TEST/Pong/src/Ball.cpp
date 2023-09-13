#include "Ball.h"

using sf::Vector2f;
using sf::RectangleShape;
using sf::FloatRect;
using sf::Time;
using sf::Color;

/* Constructor */
Ball::Ball(const float startX, const float startY)
    : m_Position{startX, startY}
    , m_Speed{ 300.f }
    , m_DirectionX{ 1.5f }
    , m_DirectionY{ 1.5f }
{
    m_Shape.setSize(Vector2f(WIDTH, WIDTH));
    m_Shape.setPosition(m_Position);
    m_Shape.setFillColor(Color::White);
}

/* Destructor */
Ball::~Ball()
{}

/* Accessors */
FloatRect Ball::getPosition() const {
    return m_Shape.getGlobalBounds();
}

RectangleShape Ball::getShape() const {
    return m_Shape;
}

float Ball::getVelocityY() const {
    return m_DirectionX;
}

void Ball::setPositionX(const float xPos) {
    m_Position.x = xPos;
}

void Ball::setPositionY(const float yPos) {
    m_Position.y = yPos;
}

/* Collision */
void Ball::reboundSides() {
    m_DirectionX = -m_DirectionX;
}

void Ball::reboundBatOrTop() {
    m_DirectionY = -m_DirectionY;
}

void Ball::reboundBottom() {
    // Position on top of screen
    m_Position.y = 0; 
    m_Position.x = 500;
}

/* Update */
void Ball::update(Time& dt) {
    // Update the ball's position
    m_Position.x += m_DirectionX * m_Speed * dt.asSeconds();
    m_Position.y += m_DirectionY * m_Speed * dt.asSeconds();

    // Move the ball
    m_Shape.setPosition(m_Position);
}
