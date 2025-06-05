#pragma once

#include "State.h"
#include "Map.h"
#include "MusicManager.h"
#include "TextureManager.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <array>

class MapViewState : public State {
public:
    explicit MapViewState(sf::RenderWindow& window);
    
    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void draw() override;

private:
    struct House {
        int id;
        int x;
        int y;
    };

  enum class TileType {
    Grass = 0,
    Bush,
    Fence,
    FenceCenter,
    FenceLeft,
    FenceRight,
    Gate,
    House1,
    House2,
    House3,
    House4,
    Lava,
    PoleLeft,
    PoleRight,
    RoadCornerLeft,
    RoadCornerRight,
    RoadStraight,
    RoadStraightHorizontal,
    RoadThreeWay,
    RoadThreeWayDown,
    Rock,
    SnowBottom,
    SnowCenter,
    SnowLeftCorner,
    SnowRightCorner,
    SnowTop,
    SnowTopLeft,
    SnowTopRight,
    Water,
    COUNT  
};

    Map m_map;
    std::vector<sf::Texture> m_textures;
    int m_tileSize = 40;
    float m_scaleFactor = 1.25f;
    std::vector<House> m_houses;
    MusicManager m_music;
    std::array<std::string, 2> m_tracks{"normal2"};
    size_t m_currentTrack = 0;

    void loadTileTextures();
    void buildFantasyMap();
    void buildRoadNetwork();
    void loadTracks();
    void cycleTracks();
    void drawTile(int x, int y, int tileID);
    void drawHouse(const House& house);
};