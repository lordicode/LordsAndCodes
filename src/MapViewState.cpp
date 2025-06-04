#include "MapViewState.h"
#include <iostream>

MapViewState::MapViewState(sf::RenderWindow& window)
: State(window)
{
    loadTileTextures();

    // not completed, will just draw grass
}

void MapViewState::handleEvent(const sf::Event& event)
{
    if (event.is<sf::Event::Closed>()) {
        m_window.close();
    }
    // TODO: WASD/panning, interactions, etc.
}

void MapViewState::update(sf::Time dt)
{
    (void)dt;
}

void MapViewState::draw()
{
    m_window.clear(sf::Color::Black);

    // Draw the tile grid: each tile is one sprite from m_textures[tileID].
    for (int y = 0; y < m_map.getHeight(); ++y) {
        for (int x = 0; x < m_map.getWidth(); ++x) {
            int tileID = m_map.getTile(x, y);
            if (tileID < 0 || tileID >= static_cast<int>(m_textures.size()))
                continue;

            sf::Sprite sprite(m_textures[tileID]);
            sprite.setPosition({
                static_cast<float>(x * m_tileSize),
                static_cast<float>(y * m_tileSize)
            });
            m_window.draw(sprite);
        }
    }

    m_window.display();
}

void MapViewState::loadTileTextures()
{
    static const std::vector<std::string> filenames = {
        "grass.png",
        "bush.png",
        "fence.png",
        "fence_center.png",
        "fence_left.png",
        "fence_right.png",
        "gate.png",
        "house_1.png",
        "house_2.png",
        "house_3.png",
        "house_4.png",
        "lava.png",
        "pole_left.png",
        "pole_right.png",
        "road_corner_left.png",
        "road_corner_right.png",
        "road_straight.png",
        "road_straight_horizontal.png",
        "road_three_way.png",
        "road_three_way_down.png",
        "rock.png",
        "snow_bottom.png",
        "snow_center.png",
        "snow_left_corner.png",
        "snow_right_corner.png",
        "snow_top.png",
        "snow_top_left.png",
        "snow_top_right.png",
        "water.png"
    };

    m_textures.resize(filenames.size());
    for (size_t i = 0; i < filenames.size(); ++i) {
        std::string fullPath = "src/Assets/MapTiles/" + filenames[i];
        if (!m_textures[i].loadFromFile(fullPath)) {
            std::cerr << "Failed to load tile texture: " << fullPath << "\n";
        }
    }

    // Derive tile size (all tile PNGs are square so this works great, e.g. 32×32 px).
    if (!m_textures.empty()) {
        m_tileSize = static_cast<int>(m_textures[0].getSize().x);
    }
}
