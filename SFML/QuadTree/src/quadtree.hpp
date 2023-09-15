#ifndef _PARTICLE_HPP
#define _PARTICLE_HPP

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <sys/types.h>
#include <memory>
#include <vector>
#include <iostream>

class Point : public sf::Drawable
{
public:
    sf::CircleShape m_shape;
    sf::Vector2f pos;

    Point(sf::Vector2f pos)
    {
        this->pos = pos;

        m_shape.setRadius(2);
        m_shape.setOrigin(2,2);
        m_shape.setPosition(pos);
        m_shape.setFillColor(sf::Color::White);
    }

    void Event()
    {
        m_shape.setFillColor(sf::Color::Red);
    }
    void Reset()
    {
        m_shape.setFillColor(sf::Color::Green);
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_shape, states);
    }
};

template <typename ITEM>
class Node : public sf::Drawable
{
    int maxItems = 4;

    sf::Vector2f m_pos;
    float m_size;
    Node* m_parent;

    std::vector<ITEM> m_itemVector;
    std::vector<Node> m_childNodes;

    bool isLeaf = true;

    bool IsInside(const sf::Vector2f& testPos)
    {
        return testPos.x > m_pos.x-m_size && testPos.x <= m_pos.x+m_size &&
               testPos.y > m_pos.y-m_size && testPos.y <= m_pos.y+m_size; 
    }

    // rectangular range
    bool RangeIntersect(const sf::Vector2f& center, float range)
    {
        /* return (center.x+range > m_pos.x-m_size || center.x-range < m_pos.x+m_size) && */
        /*        (center.y+range > m_pos.y-m_size || center.y-range < m_pos.y+m_size); */

        return (center.x+range > m_pos.x-m_size);

    }

    void InsertToChildren(const ITEM& item)
    {
        for(auto && child : m_childNodes)
        {
            auto res = child.Insert(item);

            if (res) break;
        }
    }

    void Subdivide()
    {
        isLeaf = false;

        sf::Vector2f NW{m_pos+sf::Vector2f{-0.5f,-0.5f}*m_size};
        sf::Vector2f NE{m_pos+sf::Vector2f{ 0.5f,-0.5f}*m_size};
        sf::Vector2f SW{m_pos+sf::Vector2f{-0.5f, 0.5f}*m_size};
        sf::Vector2f SE{m_pos+sf::Vector2f{ 0.5f, 0.5f}*m_size};

        m_childNodes.push_back(Node(NW, m_size/2.0f, this));
        m_childNodes.push_back(Node(NE, m_size/2.0f, this));
        m_childNodes.push_back(Node(SW, m_size/2.0f, this));
        m_childNodes.push_back(Node(SE, m_size/2.0f, this));

        for(auto && item : m_itemVector)
        {
            InsertToChildren(item);
        }
    }

public:
    sf::RectangleShape nodeShape;

    Node(sf::Vector2f pos, float size, Node* parent)
    {
        m_pos = pos;
        m_size = size;
        m_parent = parent;

        m_itemVector.reserve(maxItems);
        m_childNodes.reserve(4);

        nodeShape.setSize(sf::Vector2f{size*2,size*2});
        nodeShape.setPosition(pos-sf::Vector2f{1,1}*size);

        nodeShape.setFillColor(sf::Color::Transparent);
        nodeShape.setOutlineThickness(1);
        nodeShape.setOutlineColor(sf::Color::White);
    }

    bool Insert(const ITEM& item)
    {
        if (!IsInside(item.pos)) return false;

        if (m_itemVector.size() < maxItems) 
        {
            m_itemVector.push_back(item);
        }
        else
        {
            if (isLeaf) Subdivide();

            InsertToChildren(item);
        }

        return true;
    }

    std::vector<ITEM> Range(const sf::Vector2f center, float range)
    {
        std::vector<ITEM> items;

        if (!RangeIntersect(center,range)) return items;

        if (isLeaf) 
        {
            for (int i = 0; i < m_itemVector.size(); ++i) 
            {
                items.push_back(m_itemVector[i]);
            }
        }
        else
        {
            for(auto && child : m_childNodes)
            {
                auto childItems = child.Range(center, range);
                for (int i = 0; i < childItems.size(); ++i) 
                {
                    items.push_back(childItems[i]);
                }
            }
        }
        return items;
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(nodeShape, states);

        for(auto && child : m_childNodes)
        {
            target.draw(child, states);
        }

        for(auto && item : m_itemVector)
        {
            target.draw(item, states);
        }
    }
};

template <typename ITEM>
class QuadTree : public sf::Drawable
{
    Node<ITEM> *root = nullptr;

public:

    QuadTree(sf::Vector2f center, float size)
    {
        root = new Node<ITEM>(center, size, nullptr);
    }

    ~QuadTree()
    {
        delete root;
    }

    std::vector<ITEM> Range(const sf::Vector2f& center, float range)
    {
        return root->Range(center, range);
    }

    void Insert(const ITEM& item)
    {
        root->Insert(item);
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(*root,states);
    }
};

#endif 
