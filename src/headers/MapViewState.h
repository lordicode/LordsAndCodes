#pragma once

#include "State.h"
#include "Map.h"
#include "Enemy.h"
#include "Player.h"
#include "MusicManager.h"
#include "TextureManager.h"
#include "CombatState.h"

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <array>

class MapViewState : public State {
public:
    // Accept the shared MusicManager by reference
    MapViewState(sf::RenderWindow& window, MusicManager& music);

    void handleEvent(const sf::Event& event) override;
    void update     (sf::Time dt)         override;
    void draw       ()                    override;

private:
    struct House      { int id, x, y; };
    struct Vegetation { int x, y, tileId; };
    struct Environment{ int x, y, tileId; };

    enum class TileType {
        Grass=0, Bush, Fence, FenceCenter, FenceLeft, FenceRight, Gate,
        House1, House2, House3, House4, Lava, PoleLeft, PoleRight,
        RoadCornerLeft, RoadCornerRight, RoadStraight, RoadStraightHorizontal,
        RoadThreeWay, RoadThreeWayDown, Rock, SnowBottom, SnowCenter,
        SnowLeftCorner, SnowRightCorner, SnowTop, SnowTopLeft, SnowTopRight,
        Water, COUNT
    };

    Map                              m_map;
    std::vector<sf::Texture>         m_textures;
    int                              m_tileSize    = 40;
    float                            m_scaleFactor = 1.25f;
    std::vector<House>               m_houses;
    std::vector<Vegetation>          m_vegetation;
    std::vector<Environment>         m_environment;
    std::vector<Enemy>               m_enemies;
    sf::Font m_font;
    Player                           m_player;

    MusicManager&                    m_music;       // reference to the one in main()
    std::array<std::string,1>        m_tracks{"normal2"};
    size_t                           m_currentTrack = 0;

    // map-building helpers 
    void loadTileTextures();
    void buildFantasyMap();
    void buildRoadNetwork();
    void growVegetation();
    void placeEnvironmentTiles();
    void loadTracks();
    void loadEnemies();
    void cycleTracks();

    // drawing helpers
    void drawTile(int x, int y, int tileID);
    void drawHouse(const House& house);
    void drawVegetation();
    void drawEnvironment();
    void placeHouses();
    void drawHouses();
    void drawEnemies();
};