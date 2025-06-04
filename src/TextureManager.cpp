#include "TextureManager.h"
#include <iostream>

const sf::Texture& TextureManager::getTexture(const std::string& key, const std::string& filename)
{
    // If we already loaded “key,” return it.
    auto it = m_textures.find(key);
    if (it != m_textures.end()) {
        return it->second;
    }

    // Otherwise, load from disk and insert.
    sf::Texture tex;
    if (!tex.loadFromFile(filename)) {
        std::cerr << "Failed to load texture: " << filename << "\n";
    }
    // Move‐insert into the map and return a reference.
    auto inserted = m_textures.emplace(key, std::move(tex));
    return inserted.first->second;
}
