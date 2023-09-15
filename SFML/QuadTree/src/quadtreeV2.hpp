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
#include <utility>
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

        const int radius = 5;
        m_shape.setRadius(radius);
        m_shape.setOrigin(radius,radius);
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

struct Box
{
    sf::Vector2f m_center;
    float m_size;

    Box() = default;
    Box(sf::Vector2f center, float size)
    {
        m_center = center;
        m_size = size;
    }

    bool IsInside(const sf::Vector2f& testPos) const
    {
        return testPos.x > m_center.x-m_size && testPos.x <= m_center.x+m_size &&
               testPos.y > m_center.y-m_size && testPos.y <= m_center.y+m_size; 
    }

    bool RangeIntersect(const Box& range) const
    {
        return !(range.m_center.x-m_size > m_center.x+m_size ||
                 range.m_center.x+m_size < m_center.x-m_size ||
                 range.m_center.y-m_size > m_center.y+m_size ||
                 range.m_center.y+m_size < m_center.y-m_size);
    }
};


typedef uint16_t itemID;

struct ItemInfo
{
    sf::Vector2f itemPos;
    itemID index;
};

template <typename ITEM>
class Node : public sf::Drawable
{
    const int maxItems = 4;
    Box nodeRect;

    std::vector<ItemInfo> m_itemVector;
    std::vector<std::unique_ptr<Node>> m_childNodes;

    void InsertToChildren(const ItemInfo& item)
    {
        for(auto && child : m_childNodes)
        {
            auto inserted = child->Insert(item);
            if (inserted) break;
        }
    }

    void Subdivide()
    {
        sf::Vector2f NW{nodeRect.m_center+sf::Vector2f{-0.5f,-0.5f}*nodeRect.m_size};
        sf::Vector2f NE{nodeRect.m_center+sf::Vector2f{ 0.5f,-0.5f}*nodeRect.m_size};
        sf::Vector2f SW{nodeRect.m_center+sf::Vector2f{-0.5f, 0.5f}*nodeRect.m_size};
        sf::Vector2f SE{nodeRect.m_center+sf::Vector2f{ 0.5f, 0.5f}*nodeRect.m_size};

        m_childNodes.push_back(std::make_unique<Node>(NW, nodeRect.m_size/2.0f));
        m_childNodes.push_back(std::make_unique<Node>(NE, nodeRect.m_size/2.0f));
        m_childNodes.push_back(std::make_unique<Node>(SW, nodeRect.m_size/2.0f));
        m_childNodes.push_back(std::make_unique<Node>(SE, nodeRect.m_size/2.0f));

        for(auto && item : m_itemVector)
        {
            InsertToChildren(std::move(item));
        }
    }

    bool IsLeaf()
    {
        return m_childNodes.size() == 0;
    }

public:
    sf::RectangleShape nodeShape;

    Node(sf::Vector2f pos, float size)
    {
        this->nodeRect = Box{pos, size};

        m_itemVector.reserve(maxItems);
        m_childNodes.reserve(4);

        nodeShape.setSize(sf::Vector2f{size*2,size*2});
        nodeShape.setPosition(pos-sf::Vector2f{1,1}*size);

        nodeShape.setFillColor(sf::Color::Transparent);
        nodeShape.setOutlineThickness(1);
        nodeShape.setOutlineColor(sf::Color::White);
    }

    bool Insert(const ItemInfo& item)
    {
        if (!nodeRect.IsInside(item.itemPos)) return false;

        if (m_itemVector.size() < maxItems) 
        {
            m_itemVector.push_back(item);
        }
        else
        {
            if (IsLeaf()) Subdivide();

            InsertToChildren(item);
        }

        return true;
    }

    void Range(Box range, std::vector<itemID> &items)
    {
        if (!nodeRect.RangeIntersect(range)) return;

        if (IsLeaf()) 
        {
            for (int i = 0; i < m_itemVector.size(); ++i) 
            {
                if (range.IsInside(m_itemVector[i].itemPos)) 
                {
                    items.push_back(m_itemVector[i].index);
                }
            }
        }
        else
        {
            for(auto && child : m_childNodes)
            {
                child->Range(range, items);
            }
        }
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(nodeShape, states);

        for(auto && child : m_childNodes)
        {
            target.draw(*child.get(), states);
        }
    }
};

template <typename ITEM>
class QuadTree : public sf::Drawable
{
    std::unique_ptr<Node<ITEM>> root;

public:

    QuadTree(sf::Vector2f center, float size)
    {
        root = std::make_unique<Node<ITEM>>(center, size);
    }

    std::vector<itemID> Range(const sf::Vector2f& center, float range)
    {
        std::vector<itemID> items;
        root->Range(Box{center,range}, items);
        return items;
    }

    void Insert(const sf::Vector2f& pos, itemID index)
    {
        root->Insert(ItemInfo{pos, index});
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(*root,states);
    }
};

#endif 
