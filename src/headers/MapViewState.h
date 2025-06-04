#pragma once

#include "State.h"
#include "Map.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
//second screen after title

// The top‐down “map view” screen. Loads a grid of tile textures,
// stores them in m_textures, and draws a 2D grid of sprites.
class MapViewState : public State {
public:
    explicit MapViewState(sf::RenderWindow& window);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void draw() override;

private:
    Map                         m_map;
    std::vector<sf::Texture>    m_textures;
    int                         m_tileSize = 32; // In pixels.
    void loadTileTextures();
};
