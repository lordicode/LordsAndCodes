#include "MapViewState.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>  
#include <ctime>    
#include <sstream>
#include <string>
#include <fstream>
#include <thread>


MapViewState::MapViewState(sf::RenderWindow& window, MusicManager& music)
  : State(window)
  , m_music(music)         // store reference
  , m_map(38,24,0) {  
    if (!m_font.openFromFile("src/Assets/Fonts/ByteBounce.ttf")) {
    throw std::runtime_error("MapViewState: failed to load font");
    }
    try {
        loadTileTextures();
        loadEnemies();  
        buildFantasyMap();
        loadTracks();
        m_music.playMusic(m_tracks[m_currentTrack], false);
        m_player.setSpriteFile("lord_.png");      // first‐level sprite
        m_player.initialise({0, 0});
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize MapViewState: " << e.what() << "\n";
        throw;
    }
}

void MapViewState::handleEvent(const sf::Event& event) {
     // On left‐mouse‐click: compute tile coords and print them
    if (event.is<sf::Event::MouseButtonPressed>()) {
        auto mouseEvt = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvt->button == sf::Mouse::Button::Left) {
            int pixelX = mouseEvt->position.x;
            int pixelY = mouseEvt->position.y;

            // Convert to tile indices
            int tileX = pixelX / m_tileSize;
            int tileY = pixelY / m_tileSize;

            // Way to bruteforce where to place tiles
            if (tileX >= 0 && tileX < m_map.getWidth() &&
                tileY >= 0 && tileY < m_map.getHeight()) 
            {
                std::cout << "Clicked tile: (" 
                          << tileX << ", " << tileY << ")\n";
            } else {
                std::cout << "Click out of map bounds: (" 
                          << tileX << ", " << tileY << ")\n";
            }
        }
    }
}


void MapViewState::update(sf::Time dt)
{
    float secs = dt.asSeconds();
    for (auto& e : m_enemies) e.update(secs, m_map);

    m_player.update(secs, m_map, m_enemies);

    // When collision detected, push CombatState:
    for (auto& e : m_enemies) {
        if (!e.defeated && e.tile == m_player.getTile()) {
            m_nextState = std::make_unique<CombatState>(
                m_window, m_player, e, m_music
            );
            return;
        }
    }

    if (m_player.getTile() == sf::Vector2i(29, 17)) {
    sf::Text winText{ m_font, "YOU WON!", 72 };
    winText.setFillColor(sf::Color::Green);

    auto winSz = m_window.getSize();
    auto bounds = winText.getLocalBounds();
    float textW = bounds.size.x;
    float textH = bounds.size.y;

    winText.setPosition({
        (winSz.x - textW) * 0.5f,
        (winSz.y - textH) * 0.5f
    });

    m_window.clear(sf::Color::Black);
    m_window.draw(winText);
    m_window.display();

    std::this_thread::sleep_for(std::chrono::seconds(2));
    m_window.close();  // or set m_answer = "win" or transition to a menu
    return;
}

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
        "water.png",               // 28
        "road_corner_right_flipped.png", // 29
        "road_corner_left_flipped.png", // 30
        "crossroad.png",            // 31
        "Plant1.png",               // 32
        "Plant2.png",               // 33
        "Plant4.png",               // 34
        "sand.png",                 // 35
        "crystal_terrain.png",      // 36
        "Goblin_archivist_tower.png",// 37
        "wraith_cemetery.png",      // 38
        "medieval_workshop.png",    // 39
        "orc_mess_hall.png",         // 40
        "purply_crystal_tower.png",  // 41
        "cave.png",                   // 42
        "orc_administration.png"         // 43
    };

    m_textures.resize(filenames.size());
    const std::string basePath = "src/Assets/MapTiles/";

    for (size_t i = 0; i < filenames.size(); ++i) {
        if (!m_textures[i].loadFromFile(basePath + filenames[i])) {
            throw std::runtime_error("Failed to load tile texture: " + filenames[i]);
            continue;
        }
    }

    // Set scale factor based on first texture (for all tiles)
    if (!m_textures.empty()) {
        unsigned origSize = m_textures[0].getSize().x;
        m_scaleFactor = static_cast<float>(m_tileSize) / static_cast<float>(origSize);
    }

    
}

void MapViewState::buildRoadNetwork() {
    // Tried REALLY hard to find a math solution, but this is actually the shortest way to achieve the road look I wanted.
    const std::vector<std::pair<int, int>> horizontalTiles = {
        {1, 1}, {3, 5}, {4, 5}, {6, 5}, {8, 3}, {8, 6}, {4, 8}, {3, 8}, {3, 13},
        {4, 13}, {5, 13}, {6, 13}, {10, 6}, {12, 5}, {13, 5}, {14, 5}, {16, 11},
        {18, 13}, {17, 3}, {18, 3}, {17, 1}, {18, 1}, {19, 1}, {20, 1}, {21, 1},
        {22, 1}, {23, 1}, {26, 6}, {25, 6}, {24, 6}, {22, 8}, {22, 10},
        {23, 10}, {25, 12}, {27, 15}, {28, 15}
    };

    const std::vector<std::pair<int, int>> cornerLeftTiles = {
        {2, 8}, {16, 1}, {23, 6}, {21, 8}, {7, 3}, {7, 12}, {8, 11}, {11, 5}
    };

    const std::vector<std::pair<int, int>> cornerRightTiles = {
        {2, 1}, {9, 3}, {15, 5}, {17, 11}, {19, 3},
        {24, 1}, {25, 2}, {26, 3}, {27, 4}, {24, 10}, {26, 12}, {29, 15}
    };

    const std::vector<std::pair<int, int>> flippedCornerRightTiles = {
        {7, 13}, {11, 6}, {19, 13},
        {27, 6}, {23, 8}, {8, 12}, {9, 11}, {5, 8}
    };

    const std::vector<std::pair<int, int>> flippedCornerLeftTiles = {
        {0, 1}, {2, 5}, {2, 13}, {7, 6}, {15, 11}, {17, 13}, {21, 10},
        {24, 12}, {26, 15}, {26, 4}, {25, 3}, {24, 2}, {16, 3}
    };

    const std::vector<std::pair<int, int>> threeWayLeft = {
        {7,5}
    };

        const std::vector<std::pair<int, int>> threeWayDown = {
        {5,5}
    };


    const std::vector<std::pair<int, int>> crossroad = {
        {9, 6}
    };



    // Full list of all road tiles
    const std::vector<std::pair<int, int>> roadTiles = {
        {0, 0}, {0, 1}, {1, 1}, {2, 2}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5},
        {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {7, 4}, {7, 3}, {8, 3}, {9, 3},
        {9, 4}, {9, 5}, {9, 6}, {8, 6}, {7, 6}, {5, 6}, {5, 7}, {5, 8}, {4, 8},
        {3, 8}, {2, 8}, {2, 9}, {2, 10}, {2, 11}, {2, 12}, {2, 13}, {3, 13},
        {4, 13}, {5, 13}, {6, 13}, {7, 13}, {7, 12}, {8, 12}, {8, 11}, {9, 11},
        {9, 10}, {9, 9}, {9, 8}, {9, 7}, {10, 6}, {11, 6}, {11, 5},
        {12, 5}, {13, 5}, {14, 5}, {15, 5}, {15, 6}, {15, 7}, {15, 8}, {15, 9},
        {15, 10}, {15, 11}, {16, 11}, {17, 11}, {17, 12}, {17, 13}, {18, 13},
        {19, 13}, {19, 12}, {19, 11}, {19, 10}, {19, 9}, {19, 8}, {19, 7},
        {19, 6}, {19, 5}, {19, 4}, {19, 3}, {18, 3}, {17, 3}, {16, 3}, {16, 2},
        {16, 1}, {17, 1}, {18, 1}, {19, 1}, {20, 1}, {21, 1}, {22, 1}, {23, 1},
        {24, 1}, {24, 2}, {25, 2}, {25, 3}, {26, 3}, {26, 4}, {27, 4}, {27, 5},
        {27, 6}, {26, 6}, {25, 6}, {24, 6}, {23, 6}, {23, 7}, {23, 8}, {22, 8},
        {21, 8}, {21, 9}, {21, 10}, {22, 10}, {23, 10}, {24, 10}, {24, 11},
        {24, 12}, {25, 12}, {26, 12}, {26, 13}, {26, 14}, {26, 15}, {27, 15},
        {28, 15}, {29, 15}, {29, 16}, {29, 17}
    };

    for (const auto& [x, y] : roadTiles) {
        int tileId = 16; // Default road_straight

        for (const auto& [hx, hy] : horizontalTiles)
            if (x == hx && y == hy) { tileId = 17; break; }

        for (const auto& [fx, fy] : flippedCornerRightTiles)
            if (x == fx && y == fy) { tileId = 29; break; }

        for (const auto& [fx, fy] : flippedCornerLeftTiles)
            if (x == fx && y == fy) { tileId = 30; break; }

        for (const auto& [lx, ly] : cornerLeftTiles)
            if (x == lx && y == ly) { tileId = 14; break; }

        for (const auto& [rx, ry] : cornerRightTiles)
            if (x == rx && y == ry) { tileId = 15; break; }

        for (const auto& [rx, ry] : threeWayLeft)
            if (x == rx && y == ry) { tileId = 18; break; }

        for (const auto& [rx, ry] : threeWayDown)
            if (x == rx && y == ry) { tileId = 19; break; }

        for (const auto& [rx, ry] : crossroad)
            if (x == rx && y == ry) { tileId = 31; break; }

        m_map.setTile(x, y, tileId);
        m_map.setRoad(x, y, true);
    }
}

void MapViewState::growVegetation()
{
    const std::vector<std::pair<int,int>> treeCoords = {
        {0,7},{0,8},{0,9},{0,11},{0,12},{0,14},
        {5,10},{5,11},{6,10},
        {7,16},{7,17},
        {8,16},{8,17},
        {9,0},{9,15},{9,16},{9,17},
        {10,0},{10,1},{10,14},{10,15},
        {11,0},{11,1},{11,2},{11,3},{11,8},{11,10},{11,13},{11,14},
        {12,3},{12,13},{12,14},{12,15},{12,16},{12,17},
        {13,3},{13,8},{13,9},{13,13},{13,14},{13,15},{13,16},{13,17},
        {14,1},{14,2},{14,14},{14,15},{14,16},{14,17},
        {15,1},{15,2},{15,15},{15,16},{15,17},
        {16,16},{16,17},
        {17,7},{17,8},{17,9},{17,10},{17,16},{17,17},
        {18,16},
        {21,13},
        {22,13},{22,15},{22,16},
        {23,15},{23,16},
        {24,14},{24,16},
        {25,13},{25,14},{25,15},
        {26,16},
        {27,10},{27,13},{27,14},{27,17},
        {28,8},{28,9},{28,10},{28,11},{28,14},{28,17},
        {29,8},{29,9},{29,10},{29,11},{29,14}
    };

    const std::vector<std::pair<int,int>> flowerCoords = {
        {27,16},{28,16},{25,16},{28,13},{28,12},{29,12},{29,13},{27,12},
        {24,13},{23,14},{24,15},{17,14},{18,14},{19,14},{18,12},{18,11},
        {12,9},{12,8},{11,9},{11,7},{12,7},{13,7},{12,6},{13,6},
        {4,2},{4,1},{4,0},{5,0},{5,1},{5,2},{1,5},{1,4},{4,6},{2,7},{2,15}
    };

    const std::vector<std::pair<int,int>> bushCoords = {
        {0,2},{1,0},{2,0},{3,4},{6,4},{6,8},{6,7},{6,6},
        {1,17},{1,16},{2,16},{2,17},{3,16},{3,17},{3,15},
        {4,14},{8,14},{12,12},{20,16},{20,15},{21,15},
        {20,11},{20,7},{21,5},{16,5},{27,8},{27,7},{27,9},
        {27,11},{26,11},{24,17},{23,17}
    };


    m_vegetation.clear();

    for (auto [x,y] : treeCoords)
    {
        m_map.setTile(x, y, 0);
        int tileId = 32 + (rand() % 2);
        m_vegetation.push_back({x, y, tileId});
    }

    for (auto [x,y] : flowerCoords)
    {
        m_map.setTile(x, y, 0);
        m_vegetation.push_back({x, y, 34});
    } 
    
    for (auto [x,y] : bushCoords)
    {
        m_map.setTile(x, y, 0);
        m_vegetation.push_back({x, y, 1});   
    }

}

void MapViewState::placeEnvironmentTiles()
{
    const std::vector<std::pair<int,int>> lavaCoords = {
        {23,2},{23,3},{24,3},{24,4},{25,1},{25,4},{25,5},
        {26,1},{26,2},{26,5},
        {27,2},{27,3},
        {28,3},{28,4},{28,5}
    };

    const std::vector<std::pair<int,int>> rockCoords = {
        {22,2},{22,3},{23,4},{24,5},{28,6},{29,6},{29,5},{29,4},{29,3},
        {28,2},{27,1},{26,0},{25,0},{24,0},{1,2},{3,2},{3,1},{3,0},{18,9},{18,8},
        {20,8},{20,9},{18,10},{20,10}
    };

    const std::vector<std::pair<int,int>> waterCoords = {
        {16,10},{16,9},{16,8},{16,7},{16,6},
        {10,16},{10,17},{11,17},{11,16},{11,15},
        {1,3},{3,3},{0,17},{0,16},
        {27,0},{29,2},{25,17},{26,17},
        {14, 6}, {14, 7}, {14, 8}, {14, 9}, {14, 10},
        {28,0},{28,1},{29,1},{29,0}
        
    };

    const std::vector<std::pair<int,int>> sand = {
        {8,13},{9,12},{10,11},{8,10},{7,11},
        {6,12},{7,10},{6,11},{9,13},{10,13},{10,12}
        
    };

    const std::vector<std::pair<int,int>> snow = {
        {16,0},{17,0},{18,0},{19,0},{19,2},
        {18,2},{17,2},{20,2},{20,0}
        
    };

    const std::vector<std::pair<int,int>> crystal = {
        {26,5},{28,5},{28,6},{23,0},{22,2}        
    };

    for (auto [x,y] : lavaCoords)
        m_map.setTile(x, y, 11);

    for (auto [x,y] : rockCoords)
        m_map.setTile(x, y, 20);

    for (auto [x,y] : waterCoords)
        m_map.setTile(x, y, 28);

    for (auto [x,y] : sand)
        m_map.setTile(x, y, 35);

    for (auto [x,y] : snow)
        m_map.setTile(x, y, 25);

    for (auto [x,y] : crystal)
        m_map.setTile(x, y, 36);
}

void MapViewState::placeHouses()
{
    m_houses.clear();

    m_houses.push_back({7 ,  5,  4});   // house_1  (ID 7)
    m_houses.push_back({8 , 10,  5});   // house_2  (ID 8)
    m_houses.push_back({9 , 21,  2});   // house_3  (ID 9)
    m_houses.push_back({10, 29, 16});   // house_4  (ID 10)

    m_houses.push_back({37, 3, 2});   // goblin archivist  (ID 37)
    m_houses.push_back({38, 8, 4});   // cemetery  (ID 38)
    m_houses.push_back({39, 8, 7});   // workshop (ID 39)
    m_houses.push_back({40, 1, 12});   // mess hall  (ID 40)
    m_houses.push_back({41, 28, 4});   // crystal tower  (ID 41)
    m_houses.push_back({42, 23, 12});   // cave  (ID 42)
    m_houses.push_back({43, 4, 7});   // orc admin  (ID 43)

}

void MapViewState::loadEnemies()
{
    m_enemies.clear();

    // exact tile for every enemy sprite 
    const std::unordered_map<std::string, sf::Vector2i> fixed = {
        {"Moon_Wraith.png",          { 8,  3}},
        {"Gremlin_Archivist.png",     { 2,  2}},
        {"Sawblade_Sentinel.png",    { 8,  6}},
        {"Crystal_Seer.png",         {18,  1}},
        {"Siege_Engine.png",         {28, 15}},
        {"Mirror_Knight.png",        {15,  8}},
        {"Pit_of_Portals_Ghost.png", { 17, 13}},
        {"Goblin_Grenadier.png",     { 19,  9}},
        {"Bat_Swarm.png",            {23, 12}},
        {"Sand-Cat_Duelist.png",     {8,  12}},
        {"Troll_Quartermaster.png",  { 4, 8}},
        {"Echo_Puzzle_Ghost.png",    { 27,  5}},
        {"Ogre_Chef.png",            {2,  12}},
        {"Oracle_Obelisk.png",       {22,  9}},
        {"Frost_Warden.png",         {30,  4}}
    };

    std::ifstream in("src/Assets/GameChallenge/challenge.txt");
    if (!in) return;

    std::string header;
    std::getline(in, header);          // skip CSV header

    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty()) continue;
        std::replace(line.begin(), line.end(), '\t', ';');

        std::stringstream ss(line);
        std::vector<std::string> cols;
        std::string col;
        while (std::getline(ss, col, ';')) cols.push_back(col);
        if (cols.size() < 5) continue;

        auto trim = [](std::string& s){
            s.erase(0,  s.find_first_not_of(" \r\n"));
            s.erase(    s.find_last_not_of (" \r\n")+1);
        };
        for (auto& c : cols) trim(c);

        Enemy e;
        try { e.id = std::stoi(cols[0]); }
        catch (...) { continue; }

        e.name          = cols[1];
        e.setSpriteFile(cols[2]);
        e.intro         = cols[3];

        std::stringstream cs(cols[4]);
        while (std::getline(cs, col, ',')) {
            trim(col);
            if (!col.empty())
                try { e.challenges.push_back(std::stoi(col)); } catch (...) {}
        }

        auto pos = fixed.find(cols[2]);
        if (pos == fixed.end()) continue;          // sprite not in list
        e.tile = pos->second;   
        bool isBat = (cols[2] == "Bat_Swarm.png");
        e.initialise(e.tile, isBat);                

        m_enemies.push_back(std::move(e));
    }
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

   buildRoadNetwork();
   growVegetation();
   placeEnvironmentTiles();
   placeHouses();
}

void MapViewState::drawVegetation()
{
    for (const auto& v : m_vegetation)
        drawTile(v.x, v.y, v.tileId);  // draw on top of whatever is beneath
}

void MapViewState::drawEnvironment()
{
    for (const auto& e : m_environment)
        drawTile(e.x, e.y, e.tileId);
}

void MapViewState::drawHouses()
{
    for (const auto& h : m_houses)
        drawHouse(h);
}

void MapViewState::drawEnemies()
{
    for (auto& e : m_enemies)
    {
        // load the enemy texture
        const auto& tex = TextureManager::getInstance()
            .getTexture(e.getSpriteFile(),
                        "src/Assets/Enemy/" + e.getSpriteFile());

        sf::Sprite spr{tex};
        // scale down to tile size
        auto sz = tex.getSize();
        float s = float(m_tileSize) / float(sz.y);
        spr.setScale({s, s});
        // position at its map-tile
        spr.setPosition({
            float(e.tile.x * m_tileSize),
            float(e.tile.y * m_tileSize)}
        );

        if (e.defeated) {
            // semi-transparent white tint
            spr.setColor(sf::Color(255, 255, 255, 128));
            // rotate 90° to the right
            spr.setRotation(sf::degrees(90.f));

        }

        m_window.draw(spr);
    }
}

void MapViewState::drawTile(int x, int y, int tileID)
{
    if (tileID < 0 || tileID >= static_cast<int>(m_textures.size())) return;

    sf::Sprite   spr(m_textures[tileID]);
    sf::Vector2f scale(m_scaleFactor, m_scaleFactor);   // default

    if (tileID == 32 || tileID == 33 || tileID == 34)          // trees
    {
        sf::Vector2u ts = m_textures[tileID].getSize();
        float base = static_cast<float>(m_tileSize) / ts.x;
        scale = { base * 1.25f, base * 1.25f };
    }
    else if (tileID == 1)                                      // bushes
    {
        scale *= 0.8f;
    }
    else if (tileID == 20 || tileID == 35 || tileID == 25 || tileID == 28 || tileID >= 36)                    
    {
        sf::Vector2u ts = m_textures[tileID].getSize();
        float base = static_cast<float>(m_tileSize) / ts.x;
        scale = { base, base };
    }


    spr.setScale(scale);
    spr.setPosition(sf::Vector2f(
        static_cast<float>(x * m_tileSize),
        static_cast<float>(y * m_tileSize)
    ));

    if (tileID == 32 || tileID == 33 || tileID == 34)
        spr.move(sf::Vector2f(0.f, -6.f));                     // tree lift

    m_window.draw(spr);
}


void MapViewState::drawHouse(const House& h)
{
    const sf::Texture& tex = m_textures[h.id];
    sf::Sprite spr(tex);

    float s = static_cast<float>(m_tileSize) / tex.getSize().x;   // shrink to 1 tile, they are 120x120 normally
    spr.setScale(sf::Vector2f(s, s));
    spr.setPosition(sf::Vector2f(
        static_cast<float>(h.x * m_tileSize),
        static_cast<float>(h.y * m_tileSize)
    ));

    m_window.draw(spr);
}

void MapViewState::draw()
{
    m_window.clear(sf::Color::Black);

    for (int y = 0; y < m_map.getHeight(); ++y)
        for (int x = 0; x < m_map.getWidth(); ++x)
            drawTile(x, y, m_map.getTile(x, y));

    drawVegetation();     
    drawEnvironment();
    drawHouses(); 
    drawEnemies();
    m_player.draw(m_window, m_tileSize);

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