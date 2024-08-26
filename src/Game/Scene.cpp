#include "../../include/Game/Scene.hpp"

Scene *Scene::instance = nullptr;

Scene::Scene()
{
    if (GLOBAL::display_grid)
    {
        configureGrid(GLOBAL::cell_size, &this->grid);
    }

    this->fluid = Fluid(0, 0, 70);
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

void Scene::update(sf::Vector2f &vec, float dt)
{
    int x = floor(vec.x / GLOBAL::cell_size);
    int y = floor(vec.y / GLOBAL::cell_size);
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        this->mouseDragged(x, y);

    fluid.fluidStep();

    for (uint i = 0; i < grid.size(); i++)
    {
        for (uint j = 0; j < grid[i].size(); j++)
        {
            float d = fluid.density[fluid.IX(i, j)];
            grid[i][j].property.setFillColor(sf::Color(108, 255, 100, d));
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

void Scene::mouseDragged(int x, int y)
{
    this->fluid.addDensity(x, y, 100);
    this->fluid.addVelocity(x, y, 0.1f, 0.1f);
}