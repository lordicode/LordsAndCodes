#pragma once
#include "Entity.h"
#include "TextureManager.h"
#include <string>
#include <vector>

class Enemy : public Entity {
public:
    int                 id{};          // CSV column 1
    std::string         name;          // CSV column 2
    std::string         intro;         // CSV column 4
    std::vector<int>    challenges;    // CSV column 5 (comma-separated)

    void   setSpriteFile(const std::string& file);   // called from loader
    void   draw(sf::RenderWindow& win, int tileSize) override;

private:
    std::string spriteKey;   
};
