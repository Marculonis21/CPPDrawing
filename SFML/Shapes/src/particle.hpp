#ifndef _PARTICLE_HPP
#define _PARTICLE_HPP

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <sys/types.h>

class RoundParticle
{
    uint m_screenSize;

    float m_size;
    sf::Color m_color;

    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    
    void checkRebound()
    {
        float _x = m_position.x;
        float _y = m_position.y;

        if (_x - m_size < 0 || _x + m_size > m_screenSize) 
        {
            m_velocity.x *= -1;
        }

        if (_y - m_size < 0 || _y + m_size > m_screenSize) 
        {
            m_velocity.y *= -1;
        }
    }

    void moveParticle(sf::Time deltaTime)
    {
        m_position += m_velocity*deltaTime.asSeconds();
        checkRebound();
    }

public:
    sf::CircleShape shape;

    RoundParticle(uint screenSize, float particleSize, sf::Color particleColor, sf::Vector2f pos, sf::Vector2f vel)
    {
        m_screenSize = screenSize;
        m_size = particleSize;
        m_color = particleColor;
        m_position = pos;
        m_velocity = vel;

        this->shape.setRadius(particleSize);
        this->shape.setPosition(pos);
        this->shape.setOrigin(particleSize, particleSize);
        this->shape.setFillColor(particleColor);
    }

    void Update(sf::Time deltaTime)
    {
        moveParticle(deltaTime);
        this->shape.setPosition(m_position);
    }
};

#endif 
