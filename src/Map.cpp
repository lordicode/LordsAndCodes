#include "Map.h"

Map::Map()
: m_width(38)   
, m_height(24)  
{
    // Initialize every tile‐slot to ID 0 (we’ll treat 0 = grass).
    m_tiles.resize(m_height, std::vector<int>(m_width, 0));
}

int Map::getTile(int x, int y) const
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return 0;
    }
    return m_tiles[y][x];
}

void Map::setTile(int x, int y, int tileID)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }
    m_tiles[y][x] = tileID;
}
