#include "TextureManager.h"
#include <iostream>
#include <stdexcept>

TextureManager& TextureManager::getInstance() {
    static TextureManager instance;
    return instance;
}

bool TextureManager::preloadTexture(const std::string& key, const std::string& filename) {
    if (m_textures.find(key) != m_textures.end()) {
        return true; // Already loaded
    }

    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(filename)) {
        std::cerr << "Failed to load texture: " << filename << "\n";
        return false;
    }
    m_textures[key] = std::move(texture);
    return true;
}

const sf::Texture& TextureManager::getTexture(const std::string& key, const std::string& filename) {
    auto it = m_textures.find(key);
    if (it != m_textures.end()) {
        return *it->second;
    }

    if (!preloadTexture(key, filename)) {
        throw std::runtime_error("Failed to load texture: " + filename);
    }
    return *m_textures[key];
}

void TextureManager::clearUnusedTextures() {
    // Could implement reference counting here if needed
    // For now just clears all textures
    m_textures.clear();
}