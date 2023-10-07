#ifndef _FLUID_HPP
#define _FLUID_HPP 

#include <algorithm>
#include <vector>
#include <math.h>
#include <iostream>

#define SIZE 256

inline int IX(int x, int y)
{
    if (x < 0) x = 0; 
    if (x > SIZE-1) x = SIZE-1;

    if (y < 0) y = 0; 
    if (y > SIZE-1) y = SIZE-1;

    return (x + y*SIZE);
}

using vec = std::vector<float>;

struct FluidRect
{
    static constexpr int size = SIZE;
    static constexpr float dt = 1.0f/60.0f;
    static constexpr int iter = 4;

    float diff;
    float visc;
    
    vec s;
    vec density;

    vec Vx;
    vec Vy;

    vec Vx0;
    vec Vy0;

    FluidRect(float diffusion, float viscosity)
    {
        this->diff = diffusion;
        this->visc = viscosity;

        s.resize(size*size);
        density.resize(size*size);

        Vx.resize(size*size);
        Vy.resize(size*size);

        Vx0.resize(size*size);
        Vy0.resize(size*size);
    }

    void AddDye(int x, int y, float amount)
    {
        this->density[IX(x,y)] += amount;
        this->density[IX(x,y)] = std::min(this->density[IX(x,y)], 1.0f);
    }

    void AddVelocity(int x, int y, float amountX, float amountY)
    {
        this->Vx[IX(x,y)] += amountX;
        this->Vy[IX(x,y)] += amountY;
    }

    void Step()
    {
        diffuse(1, Vx0, Vx, visc);
        diffuse(2, Vy0, Vy, visc);
        
        project(Vx0, Vy0, Vx, Vy);
        
        advect(1, Vx, Vx0, Vx0, Vy0);
        advect(2, Vy, Vy0, Vx0, Vy0);
        
        project(Vx, Vy, Vx0, Vy0);
        
        diffuse(0, s, density, diff);
        advect(0, density, s, Vx, Vy);
    }

    void set_bnd(int b, vec& x)
    {
        int N = size;
        for(int i = 1; i < N - 1; i++) {
            x[IX(i, 0  )] = b == 2 ? -x[IX(i,   1)] : x[IX(i,   1)];
            x[IX(i, N-1)] = b == 2 ? -x[IX(i, N-2)] : x[IX(i, N-2)];
        }
        for(int j = 1; j < N - 1; j++) {
            x[IX(0  , j)] = b == 1 ? -x[IX(1  , j)] : x[IX(1  , j)];
            x[IX(N-1, j)] = b == 1 ? -x[IX(N-2, j)] : x[IX(N-2, j)];
        }
        
        x[IX(0,    0)] = 0.5f * (x[IX(1, 0)]
                               + x[IX(0, 1)]);

        x[IX(0,  N-1)] = 0.5f * (x[IX(1, N-1)]
                               + x[IX(0, N-2)]);

        x[IX(N-1,  0)] = 0.5f * (x[IX(N-2, 0)]
                               + x[IX(N-1, 1)]);

        x[IX(N-1,N-1)] = 0.5f * (x[IX(N-2, N-1)]
                               + x[IX(N-1, N-2)]);
    }

    void lin_solve(int b, vec& x, vec& x0, float a, float c)
    {
        float cRecip = 1.0 / c;
        for (int k = 0; k < iter; k++) 
        {
            for (int j = 1; j < size - 1; j++) 
            {
                for (int i = 1; i < size - 1; i++) 
                {
                    x[IX(i, j)] = (x0[IX(i, j)] + a*( x[IX(i+1, j  )]
                                                     +x[IX(i-1, j  )]
                                                     +x[IX(i  , j+1)]
                                                     +x[IX(i  , j-1)])
                                  ) * cRecip;
                }
            }
            set_bnd(b, x);
        }
    }

    void diffuse (int b, vec& x, vec& x0, float diff)
    {
        float a = dt * diff * (size - 2) * (size - 2);
        lin_solve(b, x, x0, a, 1 + 4 * a);
    }

    //could be done in parallel
    void project(vec& velocX, vec& velocY, vec& p, vec& div)
    {
        int N = this->size;

        for (int j = 1; j < N - 1; j++) 
        {
            for (int i = 1; i < N - 1; i++) 
            {
                div[IX(i, j)] = -0.5f*(velocX[IX(i+1, j  )]
                                      -velocX[IX(i-1, j  )]
                                      +velocY[IX(i  , j+1)]
                                      -velocY[IX(i  , j-1)]
                                      )/N;
                p[IX(i, j)] = 0;
            }
        }
        set_bnd(0, div); 
        set_bnd(0, p);
        lin_solve(0, p, div, 1, 4);
        
        for (int j = 1; j < N - 1; j++) {
            for (int i = 1; i < N - 1; i++) {
                velocX[IX(i, j)] -= 0.5f * (  p[IX(i+1, j  )]
                                             -p[IX(i-1, j  )]) * N;
                velocY[IX(i, j)] -= 0.5f * (  p[IX(i  , j+1)]
                                             -p[IX(i  , j-1)]) * N;
            }
        }
        set_bnd(1, velocX);
        set_bnd(2, velocY);
    }

    // could be done in parallel
    void advect(int b, vec& d, vec& d0, vec& velocX, vec& velocY)
    {
        int N = this->size;

        float i0, i1, j0, j1;

        float dtx = dt * (N - 2);
        float dty = dt * (N - 2);

        float s0, s1, t0, t1;
        float tmp1, tmp2, x, y;
        
        float ifloat, jfloat;
        int i, j;

        for(j = 1; j < N - 1; j++) 
        { 
            for(i = 1; i < N - 1; i++) 
            {
                tmp1 = dtx * velocX[IX(i, j)];
                tmp2 = dty * velocY[IX(i, j)];
                x    = i - tmp1; 
                y    = j - tmp2;
                
                if(x < 0.5f) x = 0.5f; 
                if(x > N + 0.5f) x = N + 0.5f; 
                i0 = floorf(x); 
                i1 = i0 + 1.0f;

                if(y < 0.5f) y = 0.5f; 
                if(y > N + 0.5f) y = N + 0.5f; 
                j0 = floorf(y);
                j1 = j0 + 1.0f; 
                
                s1 = x - i0; 
                s0 = 1.0f - s1; 
                t1 = y - j0; 
                t0 = 1.0f - t1;
                
                d[IX(i, j)] = 
                    s0 * ( t0 * d0[IX(i0, j0)]
                        +( t1 * d0[IX(i0, j1)]))
                   +s1 * ( t0 * d0[IX(i1, j0)]
                        +( t1 * d0[IX(i1, j1)]));
            }
        }

        set_bnd(b, d);
    }
};


#endif /* ifndef _FLUID_HPP */
