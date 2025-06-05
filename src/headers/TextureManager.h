#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <memory>

class TextureManager {
public:
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    static TextureManager& getInstance();

    const sf::Texture& getTexture(const std::string& key, const std::string& filename);
    bool preloadTexture(const std::string& key, const std::string& filename);
    void clearUnusedTextures();

private:
    TextureManager() = default;
    std::map<std::string, std::unique_ptr<sf::Texture>> m_textures;
};