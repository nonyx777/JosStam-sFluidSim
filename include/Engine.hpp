#pragma once
#include <iostream>
#include "./Game/Scene.hpp"
#include "Window.hpp"

using namespace std;

class Engine : public Window
{
private:
    sf::Vector2f currMousePos;
    sf::Vector2f prevMousePos;
public:
    // initializer functions
    void initWindow() override;

public:
    Engine();

    void update(float dt) override;
    void render(sf::RenderTarget *target) override;
    void render();

private:
    Scene *scene = Scene::getInstance();
};
