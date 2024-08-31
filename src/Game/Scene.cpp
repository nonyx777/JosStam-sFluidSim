#include "../../include/Game/Scene.hpp"

Scene *Scene::instance = nullptr;

Scene::Scene()
{
    if (GLOBAL::display_grid)
    {
        configureGrid(GLOBAL::cell_size, &this->grid);
    }
    int N = GLOBAL::window_height / GLOBAL::cell_size; // assuming height and width are of the same value
    this->fluid = Fluid(0, 0.00001f, N);
}

Scene::~Scene()
{
    delete instance;
}

Scene *Scene::getInstance()
{
    if (!instance)
        instance = new Scene();

    return instance;
}

void Scene::update(float dt)
{
    //...
}

void Scene::update(sf::Vector2f &curPos, sf::Vector2f &prevPos, float dt)
{
    int x = floor(curPos.x / GLOBAL::cell_size);
    int y = floor(curPos.y / GLOBAL::cell_size);
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        this->mouseDragged(x, y, -(curPos.x - prevPos.x), -(curPos.y - prevPos.y));

    fluid.fluidStep();

    // fade density
    for (int i = 0; i < fluid.density.size(); i++)
    {
        float d = fluid.density[i];
        fluid.density[i] = fluid.clampFloat(d - 0.1, 0, 255);
    }

    // assign color and opacity based on density
    for (uint i = 0; i < grid.size(); i++)
    {
        for (uint j = 0; j < grid[i].size(); j++)
        {
            float d = fluid.density[fluid.IX(i, j)];
            grid[i][j].property.setFillColor(sf::Color(50.f, 150.f, 250.f, fluid.clampInt(d, 0, 255)));
        }
    }
}

void Scene::render(sf::RenderTarget *target)
{
    if (this->grid.size() > 0)
    {
        for (uint i = 0; i < grid.size(); i++)
        {
            for (uint j = 0; j < grid[i].size(); j++)
            {
                target->draw(grid[i][j].property);
            }
        }
    }
}

void Scene::mouseDragged(int x, int y, float velX, float velY)
{
    this->fluid.addDensity(x, y, 255);
    this->fluid.addVelocity(x, y, velX * 0.1f, velY * 0.1f);
}