#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include "Entity.h"

class Map;
class Enemy;

class Player : public Entity {
public:
    Player();

    // Set which PNG to use
    void setSpriteFile(const std::string& file);

    // Initialise player position and stats
    void initialise(const sf::Vector2i& mapPos);

    // Update movement, XP/level, and check for combat
    void update(float dt, const Map& map, const std::vector<Enemy>& enemies);

    void takeDamage(int dmg);

    // Draw player sprite and combat placeholder text
    void draw(sf::RenderWindow& win, int tileSize) override;

    // Add XP and handle level-ups
    void gainXp(int amount);

    sf::Vector2i getTile() const { return tile; }

    int getLevel() const { return level; }
    int getArmor() const { return armor; }
    const std::string& getSpriteFile() const { return spriteKey; }
    int                 getHp()         const { return hp;        }

private:
    std::string spriteKey;
    int hp = 2;
    int armor = 0;
    int xp = 0;
    int level = 1;

    // Combat placeholder
    bool inCombat = false;
    std::string combatIntro;
    sf::Text combatText;
};