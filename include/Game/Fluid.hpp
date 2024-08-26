#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include "../Globals.hpp"

class Fluid
{
public:
    int N;
    int iter;
    float dt;

public:
    int size;
    float diff;
    float visc;

    std::vector<float> s;
    std::vector<float> density;

    std::vector<float> Vx;
    std::vector<float> Vy;

    std::vector<float> Vx0;
    std::vector<float> Vy0;

public:
    Fluid();
    Fluid(int diffusion, int viscosity, int N);

public:
    void addDensity(int x, int y, float amount);
    void addVelocity(int x, int y, float amountX, float amountY);
    void setBound(int b, std::vector<float> &x);
    void linSolve(int b, std::vector<float> &x, std::vector<float> &x0, float a, float c);
    void diffuse(int b, std::vector<float> &x, std::vector<float> &x0, float diff, float dt);
    void project(std::vector<float> &velocX, std::vector<float> &velocY, std::vector<float> &p, std::vector<float> &div);
    void advect(int b, std::vector<float> &d, std::vector<float> &d0, std::vector<float> &velocX, std::vector<float> &velocY, float dt);
    void fluidStep();

public:
    int IX(int x, int y);
    void fill(std::vector<float> &v, float value)
    {
        for (int i = 0; i < v.size(); i++)
        {
            v[i] = value;
        }
    }
};