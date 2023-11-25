#ifndef _PARTICLE_HPP
#define _PARTICLE_HPP

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstdlib>
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
        const float margin = m_radius;

        if (m_pos.y >= cPos.y + side/2 - margin) m_pos.y = cPos.y + side/2 - margin;
        if (m_pos.y  < cPos.y - side/2 + margin) m_pos.y = cPos.y - side/2 + margin;
        if (m_pos.x >= cPos.x + side/2 - margin) m_pos.x = cPos.x + side/2 - margin;
        if (m_pos.x  < cPos.x - side/2 + margin) m_pos.x = cPos.x - side/2 + margin;
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


    sf::Color heatColors[5] {sf::Color{0,0,0},
                             sf::Color{100,0,0},
                             sf::Color{255,0,0},
                             sf::Color{255,255,50},
                             sf::Color{255,255,255}};

    float heatProgressionValue[5] {0.0f,
                                   0.4f,
                                   0.6f,
                                   0.85f,
                                   1.0f};

    const float temp_min = 100;
    const float temp_max = 2000;

    sf::Color colorFromTemp()
    {
        float tempProgress = std::min(m_temp_joules/(temp_max - temp_min), 1.0f);

        int progressIndex = -1;
        for (int i = 0; i < 5; ++i) 
        {
            if (tempProgress <= heatProgressionValue[i]) 
            {
                progressIndex = i;
                break;
            }
        }

        float low = heatProgressionValue[progressIndex-1];
        float high = heatProgressionValue[progressIndex];
        sf::Color cA = heatColors[progressIndex-1];
        sf::Color cB = heatColors[progressIndex];

        float partProgress = (tempProgress-low)/(high-low);

        sf::Uint8 r = cA.r + (cB.r - cA.r)*partProgress;
        sf::Uint8 g = cA.g + (cB.g - cA.g)*partProgress;
        sf::Uint8 b = cA.b + (cB.b - cA.b)*partProgress;

        return sf::Color{r,g,b};
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

    bool heatEnabled = false;

    void Update(float deltaTime)
    {
        ApplyForce(sf::Vector2f{0,-8.0f*m_temp_joules});

        m_temp_joules = std::max(0.0f, m_temp_joules-4.0f);
        
        verletIntegration(deltaTime);
        applyConstraint();

        if (heatEnabled && std::abs(m_pos.x-400) < 200 && m_pos.y >= 790) 
        {
            m_temp_joules+=50;
        }

        if (heatEnabled && m_pos.y >= 790) 
        {
            m_temp_joules+=50;
        }

        /* float tempProgress = std::min(m_temp_joules/(2000 - 100), 1.0f); */
        /* m_radius = lerp(2, 5, tempProgress); */
        /* m_shape.setOrigin(m_radius, m_radius); */
        /* m_shape.setRadius(m_radius); */

        /* m_shape.setFillColor(sf::Color{(sf::Uint8)std::min(255.0f,m_temp_joules), 10, 10}); */
        m_shape.setFillColor(colorFromTemp());
        m_shape.setPosition(m_pos);
    }

    void ApplyForce(const sf::Vector2f& accel)
    {
        m_accel += accel;
    }

    void Event()
    {
        m_temp_joules += 0.5f;
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_shape, states);
    }
};

#endif 
