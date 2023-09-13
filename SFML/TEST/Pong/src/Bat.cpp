#include "Bat.h"
#include <SFML/Graphics/Rect.hpp>

using sf::Vector2f;
using sf::RectangleShape;
using sf::Time;
using sf::Color;
using sf::FloatRect;

/* Constructor */
Bat::Bat(const float startX, const float startY)
    : m_Position{ startX, startY }
    , m_Speed{ 1000.f }
    , m_MovingLeft{ false }
    , m_MovingRight{ false }
{
    m_Shape.setSize(Vector2f(WIDTH, HEIGHT));
    m_Shape.setPosition(m_Position);
    m_Shape.setFillColor(Color::White);
}

/* Destructor */
Bat::~Bat()
{}

/* Accessors */
FloatRect Bat::getPosition() const {
    return m_Shape.getGlobalBounds();
}

RectangleShape Bat::getShape() const {
    return m_Shape;
}

/* Behavior */

void Bat::moveLeft() {
    m_MovingLeft = true;
}

void Bat::moveRight() {
    m_MovingRight = true;
}

void Bat::stopLeft() {
    m_MovingLeft = false;
}

void Bat::stopRight() {
    m_MovingRight = false;
}

/* Update */
void Bat::update(Time& dt) {
    if (m_MovingLeft) {
        m_Position.x -= m_Speed * dt.asSeconds();
    }
    if (m_MovingRight) {
        m_Position.x += m_Speed * dt.asSeconds();
    }
    m_Shape.setPosition(m_Position);
}
