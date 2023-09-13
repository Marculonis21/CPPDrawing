#ifndef _PARTICLE_HPP
#define _PARTICLE_HPP

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <sys/types.h>
#include <cmath>

class Particle : public sf::Drawable
{
    sf::Vector2f m_init_pos;
    float m_mass;

    sf::Color m_color;
    sf::CircleShape m_shape;
    float m_radius;

    bool isActive = true;

    void keepInsideScreen()
    {
        if (m_pos.x < m_radius) m_pos.x = m_radius;
        if (m_pos.y < m_radius) m_pos.y = m_radius;
        if (m_pos.x >= 800-m_radius) m_pos.x = 800-m_radius;
        if (m_pos.y >= 800-m_radius) m_pos.y = 800-m_radius;
    }

public:
    sf::Vector2f m_pos;
    sf::Vector2f m_prev_pos;
    bool m_pinned = false;

    Particle() = default;

    Particle(int radius, sf::Color particleColor, sf::Vector2f pos, float mass)
    {
        m_color = particleColor;

        m_init_pos = pos;
        m_pos = pos;
        m_prev_pos = pos;
        m_mass = mass;
        
        m_radius = radius;

        m_shape.setOrigin(radius, radius);
        m_shape.setRadius(radius);

        m_shape.setFillColor(m_color);
        m_shape.setPosition(m_pos);
    }

    void Pin()
    {
        m_pinned = true;
    }

    void Kill()
    {
        isActive = false;
    }

    void Update(sf::Time deltaTime, float drag, const sf::Vector2f& accel, float elasticity)
    {
        if (!isActive) return;

        if (m_pinned) 
        {
            m_pos = m_init_pos;
            return;
        }

        // VERLET INTEGRATION
        auto prevPos = m_pos;
        m_pos = m_pos + (m_pos - m_prev_pos)*(1.0f-drag) + accel*(1.0f-drag)*(deltaTime.asSeconds()*deltaTime.asSeconds());
        m_prev_pos = prevPos;

        keepInsideScreen();

        /* m_shape.setPosition(m_pos); */
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (!isActive) return;

        target.draw(m_shape, states);
    }
};

#endif 
