#include "Enemy.h"

void Enemy::setSpriteFile(const std::string& file)
{
    spriteKey = file;                 
}

void Enemy::draw(sf::RenderWindow& win, int tileSize)
{
    if (defeated || spriteKey.empty()) return;

    const sf::Texture& tx =
        TextureManager::getInstance().getTexture(
            spriteKey, "src/Assets/Enemy/" + spriteKey);

    sf::Sprite sp(tx);

    float s = static_cast<float>(tileSize) / tx.getSize().x;
    sp.setScale(sf::Vector2f(s, s));                   

    sf::Vector2f pos(static_cast<float>(tile.x * tileSize),
                     static_cast<float>(tile.y * tileSize));
    sp.setPosition(pos);                                     

    win.draw(sp);
}
