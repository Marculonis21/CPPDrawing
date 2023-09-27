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
#include <exception>
#include <memory>
#include <string>
#include <sys/types.h>
#include <cmath>
#include <iostream>
#include <vector>

#include <omp.h>
#include <algorithm>
#include <execution>

#include <random>

#include "particle.hpp"
#include "grid.hpp"

/* #include "quadtreeV2.hpp" */

class ParticleSim : public sf::Drawable
{
    const sf::Vector2f m_gravity{0, 2000.f};

    std::vector<std::unique_ptr<Particle>> m_particles;

    float length(sf::Vector2f v) 
    {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    float length_squared(sf::Vector2f v) 
    {
        return v.x * v.x + v.y * v.y;
    }

    std::default_random_engine engine;
    std::uniform_real_distribution<float> distribution{0,1};

    Grid m_particleGrid;

public:
    ParticleSim() : m_particleGrid(10, 800, 800) 
    {
        auto dims = m_particleGrid.GetDimensions();
    }

    int errorCount = 10;

    void AddParticle()
    {
        const float RAD = 5;
        for (int i = 0; i < 500; ++i) 
        {
            sf::Vector2f pos{30.0f+(3*RAD)*(i%40), 200.0f+(3*RAD)*(i/40)};
            sf::Vector2f noise{distribution(engine), distribution(engine)};
            m_particles.push_back(std::make_unique<Particle>(RAD,sf::Color::Green, pos+noise, sf::Vector2f{0,0}, 1));
        }

    }

    unsigned int ParticleCount() {return m_particles.size();} const

    void MouseEvent(sf::Vector2f mousePos)
    {
        for(auto && p : m_particles)
        {
            if (length(p->m_pos - mousePos) < 50) 
            {
                p->Event();
            }
        }
    }

    bool heatEnabled = false;
    void Update(sf::Time deltaTime)
    {
        const float substeps = 8;
        const float sub_dt = deltaTime.asSeconds()/substeps;

        for (int i = 0; i < substeps; ++i) 
        {
            m_particleGrid.Clear();

            for (itemID p = 0; p < m_particles.size(); p++) 
            {
                if (std::isnan(m_particles[p]->m_pos.y)) 
                {
                    continue;
                }

                m_particles[p]->ApplyForce(m_gravity);

                m_particles[p]->heatEnabled = heatEnabled;
                m_particles[p]->Update(sub_dt);

                m_particleGrid.Insert(m_particles[p]->m_pos, p);
            }
            ParticleCollisions();
        }
    }

    void CheckCellCellItemCollisions(std::vector<itemID> cell1, std::vector<itemID> cell2)
    {
        Particle* p1;
        Particle* p2;
        for (auto id1 : cell1) 
        {
            p1 = m_particles[id1].get();
            for (auto id2 : cell2) 
            {
                p2 = m_particles[id2].get();

                if (p1 == p2) continue; 

                SolveCollision(p1, p2);
            }
        }
    }

    void ParticleCollisions()
    {
        auto dims = m_particleGrid.GetDimensions();

        for (int y = 0; y < dims.y; ++y) 
        {
            for (int x = 0; x < dims.x; ++x) 
            {
                auto current = m_particleGrid.GetCellItems(x,y);

                for (int yOff = -1; yOff < 1; ++yOff) 
                {
                    for (int xOff = -1; xOff < 1; ++xOff) 
                    {
                        // out of bounds
                        if ((x+xOff < 0 || x+xOff >= dims.x) || (y+yOff < 0 || y+yOff >= dims.y)) continue;
                        if (xOff == 0 && yOff == 0) continue; 

                        auto other = m_particleGrid.GetCellItems(x+xOff, y+yOff);
                        current.insert(current.end(), other.begin(), other.end());
                    }
                }
                CheckCellCellItemCollisions(current,current);
            }
        }
    }

    void SolveCollision(Particle* p1, Particle* p2)
    {
        float dist = length(p1->m_pos - p2->m_pos);
        if (dist > p1->m_radius*2) return;

        auto dir = (p1->m_pos - p2->m_pos)/dist;
        float delta = p1->m_radius*2 - dist;

        p1->m_pos += 0.5f * dir*delta;
        p2->m_pos -= 0.5f * dir*delta;

        /* std::cout << "temp1:" << p1->m_temp_joules << " temp2:" << p1->m_temp_joules << std::endl; */
        /* if (p1->m_temp_joules == p2->m_temp_joules) return; */

        float temp_1 = p1->m_temp_joules;
        float temp_2 = p2->m_temp_joules;

        float q1 = 0.01f*(temp_2 - temp_1);
        float q2 = 0.01f*(temp_1 - temp_2);

        p1->m_temp_joules += q1;
        p2->m_temp_joules += q2;
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        for(auto && p : m_particles)
        {
            if (std::isnan(p->m_pos.y)) continue;
            target.draw(*p);
        }
    }
};

#endif 
