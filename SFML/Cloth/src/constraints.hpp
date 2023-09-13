#ifndef _CONSTRAINT_HPP
#define _CONSTRAINT_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <sys/types.h>
#include <cmath>
#include <iostream>
#include <tuple>
#include <vector>

#include "particle.hpp"

class Constraint : public sf::Drawable
{
    Particle* m_p1;
    Particle* m_p2;

    float m_length;
    float m_breakLength;

    sf::Color m_color;

    /* sf::Vertex m_line[2]; */

    float getLength(sf::Vector2f v) 
    {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }
public:
    bool m_isActive = true;
    sf::Vertex m_line[2];

    Constraint(Particle &p1, Particle &p2, float length, float breakLength, int index)
    {
        m_p1 = &p1;
        m_p2 = &p2;
        m_length = length;
        m_breakLength = breakLength;

        m_line[0] = m_p1->m_pos;
        m_line[1] = m_p2->m_pos;
        m_line[0].color = sf::Color{255,255,255,100};
        m_line[1].color = sf::Color{255,255,255,100};
    }

    std::tuple<sf::Vector2f, sf::Vector2f> GetEndPoints()
    {
        return {m_p1->m_pos, m_p2->m_pos};
    }

    void Kill()
    {
        m_isActive = false;
    }

    void Update(sf::Time deltaTime)
    {
        if (!m_isActive) return;

        sf::Vector2f diff = m_p1->m_pos - m_p2->m_pos;
        float dist = getLength(diff);
        if (dist > m_breakLength && !(m_p1->m_pinned || m_p2->m_pinned)) 
        {
            Kill();
            return;
        }


        float diffFactor = (m_length - dist)/dist;
        sf::Vector2f offset = diff*diffFactor*0.5f;

        m_p1->m_pos += offset;
        m_p2->m_pos -= offset;

        m_line[0] = m_p1->m_pos;
        m_line[1] = m_p2->m_pos;
        m_line[0].color = sf::Color{255,255,255,100};
        m_line[1].color = sf::Color{255,255,255,100};
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (!m_isActive) return; 

        target.draw(m_line, 2 ,sf::Lines, states);
    }
};

#endif 
