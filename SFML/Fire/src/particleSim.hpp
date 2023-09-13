#ifndef _PARTICLESIM_HPP
#define _PARTICLESIM_HPP

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Mouse.hpp>
#include <sys/types.h>
#include <cmath>
#include <iostream>
#include <vector>

#include "particle.hpp"
#include <omp.h>
#include <algorithm>
#include <execution>

class ParticleSim : public sf::Drawable
{
    const sf::Vector2f m_gravity{0, 1000.f};

    sf::CircleShape m_circleConstraint;

    std::vector<Particle*> m_particles;


    float length(sf::Vector2f v) 
    {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

public:
    ParticleSim()
    {
        m_circleConstraint.setPointCount(100);
        m_circleConstraint.setRadius(300);
        m_circleConstraint.setOrigin(300,300);
        m_circleConstraint.setPosition(400,400);
        m_circleConstraint.setFillColor(sf::Color::Black);
    }

    void AddParticle()
    {
        for (int i = 0; i < 10; ++i) 
        {
            m_particles.push_back(new Particle{5,sf::Color::Green, sf::Vector2f{400+i*20.0f,300}, sf::Vector2f{2,0}, 1});
        }
    }

    void MouseEvent(sf::Vector2f mousePos)
    {
        for(auto && p : m_particles)
        {
            if (length(p->m_pos - mousePos) < 100) 
            {
                p->Event();
            }
        }
    }

    void Update(sf::Time deltaTime)
    {
        for(auto && p : m_particles)
        {
            p->ApplyForce(m_gravity);
            p->Update(deltaTime.asSeconds());
        }
        SolveCollision();
    }

    void SolveCollision()
    {
        for(auto && p : m_particles)
        {
            for(auto && other : m_particles)
            {
                if (p == other) continue;

                float dist = length(p->m_pos - other->m_pos);

                if (dist < p->m_radius*2)
                {
                    auto dir = (p->m_pos - other->m_pos)/dist;
                    float delta = p->m_radius*2 - dist;

                    p->m_pos += 0.5f * dir*delta;
                    other->m_pos -= 0.5f * dir*delta;

                    float temp_1 = p->m_temp_joules*0.0005f;
                    float temp_2 = other->m_temp_joules*0.0005f;

                    float q1 = 1 * (temp_2 - temp_1);
                    float q2 = 1 * (temp_1 - temp_2);

                    p->m_temp_joules += q1;
                    other->m_temp_joules += q2;
                }
            }
        }
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_circleConstraint);

        for(auto && p : m_particles)
        {
            target.draw(*p);
        }
    }
};

#endif 
