#ifndef _GRID_HPP
#define _GRID_HPP

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <sys/types.h>
#include <memory>
#include <utility>
#include <vector>
#include <iostream>

using itemID = uint16_t;

class Grid : public sf::Drawable
{
    using itemGrid = std::vector<std::vector<std::vector<itemID>>>;

    itemGrid m_grid;

    uint m_cellSize;

public:
    Grid(uint cellSize, uint screenWidth, uint screenHeight)
    {
        m_grid = itemGrid(screenHeight/cellSize, std::vector<std::vector<itemID>>(screenWidth/cellSize, std::vector<itemID>()));
        m_cellSize = cellSize;
    }

    sf::Vector2u GetDimensions()
    {
        return sf::Vector2u{uint(m_grid[0].size()), uint(m_grid.size())};
    }

    void Clear()
    {
        for (int y = 0; y < m_grid.size(); y++) 
        {
            for (int x = 0; x < m_grid[y].size(); x++) 
            {
                m_grid[y][x].clear();
            }
        }
    }

    void Insert(const sf::Vector2f& pos, itemID index)
    {
        int _x = pos.x/m_cellSize;
        int _y = pos.y/m_cellSize;

        m_grid[_y][_x].push_back(index);
    }

    std::vector<itemID> GetCellItems(uint x, uint y)
    {
        return m_grid[y][x];
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        /* target.draw(*root,states); */
    }
};

#endif 
