#include "../../include/Game/Fluid.hpp"
Fluid::Fluid()
{
    //...
}

Fluid::Fluid(int diffusion, int viscosity, int N)
{
    this->N = N;
    this->iter = 4;
    this->dt = 0.2f;

    this->size = N;
    this->diff = diffusion;
    this->visc = viscosity;

    this->s.resize(N * N);
    this->density.resize(N * N);

    this->Vx.resize(N * N);
    this->Vy.resize(N * N);

    this->Vx0.resize(N * N);
    this->Vy0.resize(N * N);
}

void Fluid::addDensity(int x, int y, float amount)
{
    this->density[IX(x, y)] += amount;
}

void Fluid::addVelocity(int x, int y, float amountX, float amountY)
{
    int index = IX(x, y);

    this->Vx[index] += amountX;
    this->Vy[index] += amountY;
}

void Fluid::setBound(int b, std::vector<float> &x)
{
    for (int i = 1; i < N - 1; i++)
    {
        x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
        x[IX(i, N - 1)] = b == 2 ? -x[IX(i, N - 2)] : x[IX(i, N - 2)];
    }

    for (int j = 1; j < N - 1; j++)
    {
        x[IX(0, j)] = b == 1 ? -x[IX(1, j)] : x[IX(1, j)];
        x[IX(N - 1, j)] = b == 1 ? -x[IX(N - 2, j)] : x[IX(N - 2, j)];
    }

    // corners
    x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
    x[IX(N - 1, 0)] = 0.5 * (x[IX(N - 2, 0)] + x[IX(N - 1, 1)]);
    x[IX(0, N - 1)] = 0.5f * (x[IX(0, N - 2)] + x[IX(1, N - 1)]);
    x[IX(N - 1, N - 1)] = 0.5 * (x[IX(N - 1, N - 2)] + x[IX(N - 2, N - 1)]);
}

void Fluid::linSolve(int b, std::vector<float> &x, std::vector<float> &x0, float a, float c)
{
    float cRecip = 1.f / c;
    for (int k = 0; k < iter; k++)
    {
        for (int j = 1; j < N - 1; j++)
        {
            for (int i = 1; i < N - 1; i++)
            {
                x[IX(i, j)] = (x0[IX(i, j)] + a * (x[IX(i + 1, j)] + x[IX(i - 1, j)] + x[IX(i, j - 1)] + x[IX(i, j + 1)])) * cRecip;
            }
        }
    }
    setBound(b, x);
}

void Fluid::diffuse(int b, std::vector<float> &x, std::vector<float> &x0, float diff, float dt)
{
    float a = dt * diff * (N - 2) * (N - 2);
    linSolve(b, x, x0, a, 1 + 6 * a);
}

void Fluid::project(std::vector<float> &velocX, std::vector<float> &velocY, std::vector<float> &p, std::vector<float> &div)
{
    for (int j = 1; j < N - 1; j++)
    {
        for (int i = 1; i < N - 1; i++)
        {
            div[IX(i, j)] = -0.5f * (velocX[IX(i + 1, j)] - velocX[IX(i - 1, j)] + velocY[IX(i, j + 1)] - velocY[IX(i, j - 1)]) / N;
            p[IX(i, j)] = 0;
        }
    }

    setBound(0, div);
    setBound(0, p);
    linSolve(0, p, div, 1, 6);

    for (int j = 1; j < N - 1; j++)
    {
        for (int i = 1; i < N - 1; i++)
        {
            velocX[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) * N;
            velocY[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) * N;
        }
    }

    setBound(1, velocX);
    setBound(2, velocY);
}

void Fluid::advect(int b, std::vector<float> &d, std::vector<float> &d0, std::vector<float> &velocX, std::vector<float> &velocY, float dt)
{
    float i0, i1, j0, j1;

    float dtx = dt * (N - 2);
    float dty = dt * (N - 2);

    float s0, s1, t0, t1;
    float tmp1, tmp2, tmp3, x, y;

    float Nfloat = N - 2;
    float ifloat, jfloat;
    int i, j;

    for (j = 1, jfloat = 1; j < N - 1; j++, jfloat++)
    {
        for (i = 1, ifloat = 1; i < N - 1; i++, ifloat++)
        {
            tmp1 = dtx * velocX[IX(i, j)];
            tmp2 = dty * velocY[IX(i, j)];
            x = ifloat - tmp1;
            y = jfloat - tmp2;

            if (x < 0.5f)
                x = 0.5f;
            if (x > Nfloat + 0.5f)
                x = Nfloat + 0.5f;
            i0 = floor(x);
            i1 = i0 + 1.0f;
            if (y < 0.5f)
                y = 0.5f;
            if (y > Nfloat + 0.5f)
                y = Nfloat + 0.5f;
            j0 = floorf(y);
            j1 = j0 + 1.0f;

            s1 = x - i0;
            s0 = 1.0f - s1;
            t1 = y - j0;
            t0 = 1.0f - t1;

            int i0i = i0;
            int i1i = i1;
            int j0i = j0;
            int j1i = j1;

            d[IX(i, j)] =

                s0 * (t0 * d0[IX(i0i, j0i)] + t1 * d0[IX(i0i, j1i)]) + s1 * (t0 * d0[IX(i1i, j0i)] + t1 * d0[IX(i1i, j1i)]);
        }
    }
    setBound(b, d);
}

void Fluid::fluidStep()
{
    float visc = this->visc;
    float diff = this->diff;
    float dt = this->dt;

    diffuse(1, Vx0, Vx, visc, dt);
    diffuse(2, Vy0, Vy, visc, dt);

    project(Vx0, Vy0, Vx, Vy);

    advect(1, Vx, Vx0, Vx0, Vy0, dt);
    advect(2, Vy, Vy0, Vx0, Vy0, dt);

    project(Vx, Vy, Vx0, Vy0);

    diffuse(0, s, density, diff, dt);
    advect(0, density, s, Vx, Vy, dt);
}

int Fluid::IX(int x, int y)
{
    int v = x * N + y;
    return clampInt(v, 0, (N*N)-2);
}