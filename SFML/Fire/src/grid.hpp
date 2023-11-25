#ifndef _GRID_HPP
#define _GRID_HPP

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstdint>
#include <sys/types.h>
#include <memory>
#include <type_traits>
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
        m_grid = itemGrid((screenHeight/cellSize)+1, std::vector<std::vector<itemID>>((screenWidth/cellSize)+1, std::vector<itemID>()));
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
    
    void UpdateParticle(const sf::Vector2f& pos, const sf::Vector2f& prev_pos, itemID index)
    {
        int _x = pos.x/m_cellSize;
        int _y = pos.y/m_cellSize;
        
        int _x_prev = prev_pos.x/m_cellSize;
        int _y_prev = prev_pos.y/m_cellSize;

        if (_x == _x_prev && _y == _y_prev) return;

        /* auto g_old = m_grid[_y_prev][_x_prev]; */

        auto it = std::find(m_grid[_y_prev][_x_prev].begin(), m_grid[_y_prev][_x_prev].end(), index);
        if (it == m_grid[_y_prev][_x_prev].end()) 
        {
            /* printf("%.2f:%.2f %.2f:%.2f\n", _x, _y, _x_prev, _y_prev); */
            printf("%d:%d %d:%d\n", _x, _y, _x_prev, _y_prev);
            std::string out;
            std::cin >> out;
            std::cout << out << std::endl;
            std::cout << "the fuck" << std::endl;;
            return;
        }

        auto id = it - m_grid[_y_prev][_x_prev].begin();

        /* if (id != m_grid[_y_prev][_x_prev].size()-1) */
        /* { */
        /*     std::swap(m_grid[_y_prev][_x_prev][id], */ 
        /*               m_grid[_y_prev][_x_prev][m_grid[_y_prev][_x_prev].size()-1]); */
        /* } */

        m_grid[_y_prev][_x_prev].erase(it);
        /* m_grid[_y_prev][_x_prev].pop_back(); */
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
