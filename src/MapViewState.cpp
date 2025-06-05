#include "MapViewState.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>

MapViewState::MapViewState(sf::RenderWindow& window)
    : State(window),
      m_map(38, 24, 0) {  
    
    try {
        loadTileTextures();
        buildFantasyMap();
        loadTracks();
        m_music.playMusic(m_tracks[m_currentTrack], false);
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize MapViewState: " << e.what() << "\n";
        throw;
    }
}

void MapViewState::handleEvent(const sf::Event& event) {
 // TODO
}


void MapViewState::update(sf::Time dt) {
    (void)dt; 
    cycleTracks();
}

void MapViewState::loadTileTextures() {
    const std::vector<std::string> filenames = {
        "grass.png",               //  0
        "bush.png",                //  1
        "fence.png",               //  2
        "fence_center.png",        //  3
        "fence_left.png",          //  4
        "fence_right.png",         //  5
        "gate.png",                //  6
        "house_1.png",             //  7
        "house_2.png",             //  8
        "house_3.png",             //  9
        "house_4.png",             // 10
        "lava.png",                // 11
        "pole_left.png",           // 12
        "pole_right.png",          // 13
        "road_corner_left.png",    // 14
        "road_corner_right.png",   // 15
        "road_straight.png",       // 16 
        "road_straight_horizontal.png", // 17 
        "road_three_way.png",      // 18
        "road_three_way_down.png", // 19
        "rock.png",                // 20
        "snow_bottom.png",         // 21
        "snow_center.png",         // 22
        "snow_left_corner.png",    // 23
        "snow_right_corner.png",   // 24
        "snow_top.png",            // 25
        "snow_top_left.png",       // 26
        "snow_top_right.png",      // 27
        "water.png"                // 28
    };

    m_textures.resize(filenames.size());
    const std::string basePath = "src/Assets/MapTiles/";

    for (size_t i = 0; i < filenames.size(); ++i) {
        if (!m_textures[i].loadFromFile(basePath + filenames[i])) {
            throw std::runtime_error("Failed to load tile texture: " + filenames[i]);
        }
    }

    // Set scale factor based on first texture (for all tiles)
    if (!m_textures.empty()) {
        unsigned origSize = m_textures[0].getSize().x;
        m_scaleFactor = static_cast<float>(m_tileSize) / static_cast<float>(origSize);
    }

    // Scale houses, they are 120x120, so they need to have a differenet scale, that is TODO

    
}

void MapViewState::buildFantasyMap() {
    const int width = m_map.getWidth();
    const int height = m_map.getHeight();
    const int midX = width / 2;

    // Fill map with grass (ID 0)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            m_map.setTile(x, y, 0);
        }
    }

   // TODO
}

void MapViewState::buildRoadNetwork() {
//TODO
}

void MapViewState::drawTile(int x, int y, int tileID) {
    if (tileID < 0 || tileID >= static_cast<int>(m_textures.size())) return;

    sf::Sprite spr(m_textures[tileID]);
    spr.setScale(sf::Vector2f(m_scaleFactor, m_scaleFactor));
    spr.setPosition(
        sf::Vector2f(
            static_cast<float>(x * m_tileSize),
            static_cast<float>(y * m_tileSize)
        )
    );
    m_window.draw(spr);
}


void MapViewState::draw() {
    m_window.clear(sf::Color::Black);

    // Draw base tiles
    for (int y = 0; y < m_map.getHeight(); ++y) {
        for (int x = 0; x < m_map.getWidth(); ++x) {
            const int tileID = m_map.getTile(x, y);
            
            drawTile(x, y, tileID);
        }
    }

    m_window.display();
}

void MapViewState::loadTracks() {
    // if (!m_music.loadMusic("normal", "src/Assets/Music/normal.wav")) {
    //     throw std::runtime_error("Failed to load music: normal.wav");
    // }
    if (!m_music.loadMusic("normal2", "src/Assets/Music/normal2.wav")) {
        throw std::runtime_error("Failed to load music: normal2.wav");
    }
}

void MapViewState::cycleTracks() {
    const std::string& current = m_tracks[m_currentTrack];
    if (!m_music.isMusicPlaying(current)) {
        m_currentTrack = (m_currentTrack + 1) % m_tracks.size();
        m_music.playMusic(m_tracks[m_currentTrack], false);
    }
}