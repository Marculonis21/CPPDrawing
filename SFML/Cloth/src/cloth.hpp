#ifndef _CLOTH_HPP
#define _CLOTH_HPP

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
#include <vector>

#include "particle.hpp"

#include "constraints.hpp"

class Cloth : public sf::Drawable
{
    const sf::Vector2f m_gravity{0, 500.f};
    const float m_drag = 0.01f;
    float m_elasticity = .75f;
    float m_breakLengthFactor = 4.0f;

    std::vector<Particle*> m_points;
    std::vector<Constraint*> m_strings;

    float getLength(sf::Vector2f v) 
    {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

public:
    std::vector<sf::Vertex> vertexArray;
    int vertexCount = 0;

    Cloth(int widht, int height, float spacing, sf::Vector2f startPos)
    {
        int cIndex = 0;

        for (size_t y = 0; y < height; ++y) 
        {
            for (size_t x = 0; x < widht; ++x) 
            {
                Particle *particle = new Particle{0, sf::Color::White, startPos+sf::Vector2f{spacing*x, spacing*y}, 1};

                if (x != 0) 
                {
                    Particle *left = m_points[m_points.size() - 1];
                    Constraint *c = new Constraint{*particle, *left, spacing, spacing*m_breakLengthFactor, cIndex++};
                    m_strings.push_back(c);
                }

                if (y != 0) 
                {
                    Particle *up = m_points[x + (y-1)*(widht)];
                    Constraint *c = new Constraint{*particle, *up, spacing, spacing*m_breakLengthFactor, cIndex++};
                    m_strings.push_back(c);
                }

                /* if ((x == 0 || x == widht-1) && y==0) */
                /* { */
                /*     particle->Pin(); */
                /* } */

                if (x%4 == 0 && y==0)
                {
                    particle->Pin();
                }

                m_points.push_back(particle);
            }
        }
    }

    void MouseEvents(const sf::Vector2f &mousePos, const sf::Vector2f &prevMousePos, float mouseRadius, bool LHOLD, bool RHOLD)
    {
        if (!LHOLD && !RHOLD) return;

        if (LHOLD) 
        {
            bool p1In;
            bool p2In;

            for(auto && s : m_strings)
            {
                auto [p1Pos, p2Pos] = s->GetEndPoints();

                p1In = (getLength(p1Pos - mousePos) <= mouseRadius);
                p2In = (getLength(p2Pos - mousePos) <= mouseRadius);

                if (p1In && p2In) 
                {
                    s->Kill();
                }
            }
        }

        if (RHOLD) 
        {
            auto diff = mousePos - prevMousePos;

            if (diff.x >  m_elasticity) diff.x =  m_elasticity; 
            if (diff.y >  m_elasticity) diff.y =  m_elasticity; 
            if (diff.x < -m_elasticity) diff.x = -m_elasticity; 
            if (diff.y < -m_elasticity) diff.y = -m_elasticity; 

            for(auto && p : m_points)
            {
                if (getLength(p->m_pos - mousePos) <= mouseRadius) 
                {
                    p->m_prev_pos -= diff;
                }

            }
        }
    }

    void Update(sf::Time deltaTime)
    {
        for(auto && point : m_points)
        {
            point->Update(deltaTime, m_drag, m_gravity, m_elasticity);
        }
        for(auto && string : m_strings)
        {
            string->Update(deltaTime);
        }
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        /* for(auto && point : m_points) */
        /* { */
        /*     target.draw(*point); */
        /* } */

        for(auto && string : m_strings)
        {
            target.draw(*string);
        }
        std::vector<sf::Vertex> vertexArray;
        int vertexCount = 0;

        for(auto && string : m_strings)
        {
            if (string->m_isActive) 
            {
                vertexArray.push_back(string->m_line[0]);
                vertexArray.push_back(string->m_line[1]);
                vertexCount += 2;
            }
        }

        target.draw(&vertexArray[0], vertexCount, sf::Lines, states);
    }
};

#endif 
