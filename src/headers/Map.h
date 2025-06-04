#pragma once

#include <vector>

// A simple tile‐grid where each int is a “tile ID” (index into a texture array).
class Map {
public:
    Map();

    int  getWidth()  const { return m_width;  }
    int  getHeight() const { return m_height; }
    int  getTile(int x, int y) const;
    void setTile(int x, int y, int tileID);

private:
    int m_width;
    int m_height;
    std::vector<std::vector<int>> m_tiles;
};
