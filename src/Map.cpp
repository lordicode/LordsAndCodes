#include "Map.h"
#include <algorithm>

Map::Map(int width, int height, int defaultTile) 
    : m_width(width), m_height(height) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Map dimensions must be positive");
    }
    resize(width, height, defaultTile);
}

void Map::validateCoordinates(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        throw std::out_of_range("Map coordinates out of range");
    }
}

int Map::getTile(int x, int y) const {
    validateCoordinates(x, y);
    return m_tiles[y][x];
}

void Map::setTile(int x, int y, int tileID) {
    validateCoordinates(x, y);
    m_tiles[y][x] = tileID;
}

void Map::resize(int newWidth, int newHeight, int defaultTile) {
    if (newWidth <= 0 || newHeight <= 0) {
        throw std::invalid_argument("Map dimensions must be positive");
    }

    m_width = newWidth;
    m_height = newHeight;
    m_tiles.resize(m_height, std::vector<int>(m_width, defaultTile));
}

void Map::fill(int tileID) {
    for (auto& row : m_tiles) {
        std::fill(row.begin(), row.end(), tileID);
    }
}