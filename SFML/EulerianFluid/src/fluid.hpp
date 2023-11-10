#ifndef _FLUID_HPP
#define _FLUID_HPP

#include <SFML/System/Vector2.hpp>
#include <sys/types.h>
#include <vector>

class FluidGrid
{
    uint m_cellSize;
    sf::Vector2i m_dims;

    std::vector<std::vector<float>> uVel;
    std::vector<std::vector<float>> vVel;

    std::vector<std::vector<float>> density;

    std::vector<std::vector<float>> p;

    std::vector<std::vector<int>> cellType;


    void VelocityUpdate(float dt, const sf::Vector2f& force)
    {
        for (int i = 0; i < m_dims.x; ++i) 
        {
            for (int j = 0; j < m_dims.y; ++j) 
            {
                if (cellType[j][i] == 0) 
                {
                    uVel[j][i] += dt*force.x; 
                    vVel[j][i] += dt*force.y; 
                }
            }
        }
    }

    void Project(float dt, bool overrelaxFlag)
    {
        const int iter = 4;

        int i, j, sSum;
        float d;

        float density;

        const float overrelaxConst = 1.9f;

        for (int n = 0; n < iter; ++n) 
        {
            for (i = 0; i < m_dims.x; ++i) 
            {
                for (j = 0; j < m_dims.y; ++j) 
                {
                    p[j][i] = 0;

                    d = uVel[j][i+1] - uVel[j][i] + vVel[j+1][i] - vVel[j][i];
                    if (overrelaxFlag) d*=overrelaxConst;

                    sSum = cellType[j][i+1] + cellType[j][i-1] + cellType[j+1][i] + cellType[j-1][i];

                    uVel[j][i]   += d*cellType[j][i-1]/sSum;
                    uVel[j][i+1] -= d*cellType[j][i+1]/sSum;
                    vVel[j][i]   += d*cellType[j-1][i]/sSum;
                    vVel[j+1][i] -= d*cellType[j+1][i]/sSum;

                    p[j][i] += d/sSum * (density*m_cellSize)/dt;
                }
            }
        }
    }

    float Sample()
    {
    }

    void Advection(float dt)
    {
        std::vector<std::vector<float>> _uVel = std::vector<std::vector<float>>(m_dims.y, std::vector<float>(m_dims.x));
        std::vector<std::vector<float>> _vVel = std::vector<std::vector<float>>(m_dims.y, std::vector<float>(m_dims.x));


        int i,j;
        for (i = 0; i < m_dims.x; ++i) 
        {
            for (j = 0; j < m_dims.y; ++j) 
            {
            }
        }
    }

public:
    FluidGrid(uint cellSize, sf::Vector2i gridDims)
    {
        m_cellSize = cellSize;
        m_dims = gridDims;

        /* uVel = std::vector<std::vector<float>>(m_dims.y,   std::vector<float>(m_dims.x+1)); */
        /* vVel = std::vector<std::vector<float>>(m_dims.y+1, std::vector<float>(m_dims.x)); */
        uVel = std::vector<std::vector<float>>(m_dims.y, std::vector<float>(m_dims.x));
        vVel = std::vector<std::vector<float>>(m_dims.y, std::vector<float>(m_dims.x));

        density= std::vector<std::vector<float>>(m_dims.y, std::vector<float>(m_dims.x));

        p = std::vector<std::vector<float>>(m_dims.y, std::vector<float>(m_dims.x));

        cellType = std::vector<std::vector<int>>(m_dims.y, std::vector<int>(m_dims.x, 1));

        for (int i = 0; i < m_dims.x; ++i) 
        {
            cellType[0         ][i] = 0;
            cellType[m_dims.y-1][i] = 0;
        }

        for (int j = 0; j < m_dims.y; ++j) 
        {
            cellType[j][0         ] = 0;
            cellType[j][m_dims.x-1] = 0;
        }
    }
};

#endif 
