#include "Player.h"
#include "Map.h"
#include "Enemy.h"
#include "TextureManager.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/String.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <vector>
#include <cmath>
#include <iostream>

// Shared font for combat text
static sf::Font combatFont;

Player::Player()
    : combatText(combatFont)
{
    // Load font once
    if (!combatFont.openFromFile("src/Assets/Fonts/ByteBounce.ttf")) {
        std::cerr << "Warning: failed to load font for Player combat text\n";
    }
    combatText.setCharacterSize(16);
    combatText.setFillColor(sf::Color::White);
}
void Player::setSpriteFile(const std::string& file) {
    spriteKey = file;
}

void Player::initialise(const sf::Vector2i& mapPos) {
    tile = mapPos;      
    hp = 1;
    armor = 0;
    xp = 0;
    level = 1;
    inCombat = false;
}

void Player::update(float dt, const Map& map, const std::vector<Enemy>& enemies) {
    static bool moved = false;
    sf::Vector2i dir{0, 0};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))      dir.x = -1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dir.x = +1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    dir.y = -1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  dir.y = +1;

    if (dir != sf::Vector2i{0, 0}) {
        if (!moved) {
            sf::Vector2i next = tile + dir;
            if (map.isRoad(next.x, next.y)) {
                tile = next;
            }
            moved = true;
        }
    } else {
        moved = false;
    }

    // Check for combat collision
    inCombat = false;
    for (const auto& e : enemies) {
        if (!e.defeated && e.tile == tile) {
            inCombat = true;
            combatIntro = e.intro;
            combatText.setString("⚔ " + combatIntro);
            combatText.setPosition(sf::Vector2f(
                1200.f - 300.f,
                700.f - 50.f
            ));
            break;
        }
    }
}

void Player::draw(sf::RenderWindow& win, int tileSize) {
    if (!spriteKey.empty()) {
        const std::string assetPath = "src/Assets/Player/" + spriteKey;
        const auto& tx = TextureManager::getInstance().getTexture(spriteKey, assetPath);
        sf::Sprite sp(tx);
        float scaleFactor = static_cast<float>(tileSize) / tx.getSize().x;
        sp.setScale(sf::Vector2f(scaleFactor, scaleFactor));
        sp.setPosition(sf::Vector2f(
            static_cast<float>(tile.x * tileSize),
            static_cast<float>(tile.y * tileSize)
        ));
        win.draw(sp);
    }

    if (inCombat) {
        win.draw(combatText);
    }
}

void Player::gainXp(int amount) {
    xp += amount;
    while (xp >= 5) {
        xp -= 5;
        ++level;
        ++armor;  // extra armor per level
        spriteKey = "lord" + std::to_string(level) + ".png";
    }
}