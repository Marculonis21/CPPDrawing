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
    sf::Color m_color;

    float m_mass;
    sf::Vector2f m_accel;

    sf::CircleShape m_shape;

    float lengthSquared(sf::Vector2f v)
    {
        return v.x*v.x + v.y*v.y;
    }

    float length(sf::Vector2f v)
    {
        return std::sqrt(v.x*v.x + v.y*v.y);
    }

    void verletIntegration(float deltaTime)
    {
        // VERLET INTEGRATION
        auto prevPos = m_pos;
        m_pos = 2.0f*m_pos - m_prev_pos + m_accel*(deltaTime*deltaTime);
        m_prev_pos = prevPos;

        m_accel = {0,0};
    }

    void circleConstraint(const sf::Vector2f &cPos, float radius)
    {
        float cRadius = radius - m_radius;

        sf::Vector2f to_obj = m_pos - cPos;
        float dist = length(to_obj);

        if (dist > cRadius) 
        {
            auto dir = to_obj/dist;
            m_pos = cPos + dir*cRadius;
        }
    }

    void squareConstraint(const sf::Vector2f &cPos, float side)
    {
        if (m_pos.x >= cPos.x + side/2 - m_radius) m_pos.x = cPos.x + side/2 - m_radius;
        if (m_pos.x  < cPos.x - side/2 + m_radius) m_pos.x = cPos.x - side/2 + m_radius;
        if (m_pos.y >= cPos.y + side/2 - m_radius) m_pos.y = cPos.y + side/2 - m_radius;
        if (m_pos.y  < cPos.y - side/2 + m_radius) m_pos.y = cPos.y - side/2 + m_radius;
    }

    void applyConstraint()
    {
        /* circleConstraint(sf::Vector2f{400,400},300); */
        squareConstraint(sf::Vector2f{400,400},800);
    }

    float lerp(float v0, float v1, float t) 
    {
        return (1 - t) * v0 + t * v1;
    }

public:
    float m_radius;
    sf::Vector2f m_pos;
    sf::Vector2f m_prev_pos;

    float m_temp_joules;

    Particle() = default;

    Particle(int radius, sf::Color particleColor, sf::Vector2f pos, sf::Vector2f startVel, float mass)
    {
        m_color = particleColor;

        m_pos = pos;
        m_prev_pos = pos-startVel;
        m_mass = mass;
        
        m_radius = radius;

        m_shape.setOrigin(radius, radius);
        m_shape.setRadius(radius);
        m_shape.setFillColor(m_color);
        m_shape.setPosition(m_pos);
    }

    void Update(float deltaTime)
    {
        /* ApplyForce(sf::Vector2f{0,-10.0f*m_temp_joules}); */
        applyConstraint();
        verletIntegration(deltaTime);

        m_shape.setFillColor(sf::Color{(sf::Uint8)m_temp_joules, 10, 10});
        m_shape.setPosition(m_pos);
    }

    void ApplyForce(const sf::Vector2f& accel)
    {
        m_accel += accel;
    }

    void Event()
    {
        m_temp_joules = 200.0f;
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_shape, states);
    }
};

#endif 
