#pragma once
#include <SFML/Graphics.hpp>

class Entity {
public:
    sf::Vector2i tile;          
    bool         defeated{false};

    virtual void draw(sf::RenderWindow& win, int tileSize) = 0;
    virtual ~Entity() = default;
};
