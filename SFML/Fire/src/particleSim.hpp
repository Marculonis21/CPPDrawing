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
#include <memory>
#include <sys/types.h>
#include <cmath>
#include <iostream>
#include <vector>

#include <omp.h>
#include <algorithm>
#include <execution>

#include <random>

#include "particle.hpp"
#include "quadtreeV2.hpp"

class ParticleSim : public sf::Drawable
{
    const sf::Vector2f m_gravity{0, 1000.f};

    std::vector<std::unique_ptr<Particle>> m_particles;

    float length(sf::Vector2f v) 
    {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    float length_squared(sf::Vector2f v) 
    {
        return v.x * v.x + v.y * v.y;
    }

    std::default_random_engine engine{};
    std::uniform_real_distribution<float> distribution{-1,1};


public:
    ParticleSim()
    {
    }

    void AddParticle()
    {
        for (int i = 0; i < 400; ++i) 
        {
            sf::Vector2f pos{200+20.0f*(i%20), 200+20.0f*(i/20.0f)};
            sf::Vector2f noise{distribution(engine), distribution(engine)};
            m_particles.push_back(std::make_unique<Particle>(3,sf::Color::Green, pos+noise, sf::Vector2f{0,0}, 1));
        }
    }

    unsigned int ParticleCount() {return m_particles.size();} const

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
        QuadTree tree{sf::Vector2f{400,400}, 400};

        for (itemID p = 0; p < m_particles.size(); p++) 
        {
            m_particles[p]->ApplyForce(m_gravity);
            m_particles[p]->Update(deltaTime.asSeconds());

            tree.Insert(m_particles[p]->m_pos, p);
        }

        SolveCollision(tree);
    }

    void SolveCollision(const QuadTree& tree)
    {
        // find collisions

        for(auto && p : m_particles)
        {
            //find collisions
            auto colCandidates = tree.Range(p->m_pos, p->m_radius*2.0f);
            
            for(auto && otherID : colCandidates)
            {
                auto other = m_particles[otherID].get();

                if (p.get() == other) continue;

                float dist = length_squared(p->m_pos - other->m_pos);

                // solve collisions
                if (dist < (p->m_radius*2)*(p->m_radius*2))
                {
                    float _dist = length(p->m_pos - other->m_pos);

                    auto dir = (p->m_pos - other->m_pos)/_dist;
                    float delta = p->m_radius*2 - _dist;

                    p->m_pos += 0.5f * dir*delta;
                    other->m_pos -= 0.5f * dir*delta;

                    float temp_1 = p->m_temp_joules*0.1f;
                    float temp_2 = other->m_temp_joules*0.1f;

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
        for(auto && p : m_particles)
        {
            target.draw(*p);
        }
    }
};

#endif 
