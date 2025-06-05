#pragma once

#include <vector>
#include <stdexcept>

class Map {
public:
    Map(int width = 38, int height = 24, int defaultTile = 0);
    
    int getWidth() const noexcept { return m_width; }
    int getHeight() const noexcept { return m_height; }
    
    int getTile(int x, int y) const;
    void setTile(int x, int y, int tileID);
    
    void resize(int newWidth, int newHeight, int defaultTile = 0);
    void fill(int tileID);

private:
    int m_width;
    int m_height;
    std::vector<std::vector<int>> m_tiles;

    void validateCoordinates(int x, int y) const;
};