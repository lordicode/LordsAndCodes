#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <string>

// each texture loads once and is reused
// You can call:  const sf::Texture& tex = manager.getTexture("grass", "src/Assets/MapTiles/grass.png");
class TextureManager {
public:
    // If this key is already loaded, returns the stored texture.
    // Otherwise it loads from disk and stores it.
    const sf::Texture& getTexture(const std::string& key, const std::string& filename);

private:
    std::map<std::string, sf::Texture> m_textures;
};
