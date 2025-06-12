#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>
#include "Entity.h"

class Map;

class Enemy : public Entity {
public:
    int  id{};          
    std::string name;          
    std::string intro;         
    std::vector<int> challenges;

    int  hp       = 1;
    int  maxHp    = 1;
    bool defeated = false;

    // set which PNG to use
    void setSpriteFile(const std::string& file);

    // lifecycle
    void initialise(const sf::Vector2i& mapPos,
                    bool                isBatSwarm = false,
                    int                 maxSteps   = 3);
    void update(float dt, const Map& map);
    void draw  (sf::RenderWindow& win, int tileSize) override;

private:
    std::string spriteKey;
    sf::Vector2i originTile;
    int maxSteps = 3;       // max outbound steps
    int stepsLeft = 0;       // how many to go outbound/return
    bool returning = false;   // outbound vs return
    std::vector<sf::Vector2i> path;                 // stores each step

    // timing & pulse
    float moveTimer = 0.f;
    float pulseClock = 0.f;
    float scale = 1.f;

};
