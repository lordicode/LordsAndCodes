#include "Enemy.h"
#include "Map.h"
#include "TextureManager.h"

#include <cstdlib>
#include <cmath>

static int  randInt(int lo,int hi){ return lo + std::rand()%(hi-lo+1); }

void Enemy::setSpriteFile(const std::string& file) {
    spriteKey = file;
}

void Enemy::initialise(const sf::Vector2i& mapPos,
                       bool                isBatSwarm,
                       int                 maxSt)
{
    originTile = tile = mapPos;
    path.clear();
    returning  = false;
    maxSteps   = std::max(1, std::min(5, maxSt));  // clamp 1..5
    stepsLeft  = randInt(1, maxSteps);

    // health
    if(isBatSwarm){ hp = maxHp = 2; }
    else {
        int dist = std::abs(tile.x)+std::abs(tile.y);
        int calc = dist/6 + 1;
        hp = maxHp = std::min(6, calc);
    }
    defeated = false;

    moveTimer  = randInt(3,7);  // first move in 3–7s
    pulseClock = 0.f;
    scale      = 1.f;
}

void Enemy::update(float dt, const Map& map)
{
    if(defeated) return;

    // pulse
    pulseClock += dt;
    scale = 1.f + 0.01f * std::sin(3.14f * pulseClock);

    // off-road - only pulse
    if(!map.isRoad(originTile.x, originTile.y)) return;

    // timer
    moveTimer -= dt;
    if(moveTimer > 0.f) return;
    moveTimer = randInt(3,7);

    // decide next tile
    sf::Vector2i next;
    if(!returning){
        // outbound: pick a random adjacent road tile
        std::vector<sf::Vector2i> nbrs;
        auto tryAdd=[&](int dx,int dy){
            sf::Vector2i c{tile.x+dx,tile.y+dy};
            if(map.isRoad(c.x,c.y)) nbrs.push_back(c);
        };
        tryAdd(+1,0); tryAdd(-1,0);
        tryAdd(0,+1); tryAdd(0,-1);

        if(nbrs.empty()){
            // nowhere to go - start return immediately
            returning = true;
            stepsLeft = (int)path.size();
            return;
        }
        int idx = std::rand()%nbrs.size();
        path.push_back(tile);
        next = nbrs[idx];
        --stepsLeft;
        if(stepsLeft==0){
            returning = true;
            stepsLeft = (int)path.size();
        }
    }
    else {
        // return 
        next = path.back();
        path.pop_back();
        --stepsLeft;
        if(stepsLeft==0){
            returning = false;
            stepsLeft = randInt(1, maxSteps);
            path.clear();
        }
    }

    tile = next;
}

void Enemy::draw(sf::RenderWindow& win, int tileSize)
{
    if(spriteKey.empty()) return;
    const std::string assetPath = "src/Assets/Enemy/" + spriteKey;
    const auto& tx = TextureManager::getInstance()
                        .getTexture(spriteKey, assetPath);
    sf::Sprite sp(tx);
    float s = tileSize/float(tx.getSize().x)*scale;
    sp.setScale(sf::Vector2f{s,s});
    sp.setPosition(sf::Vector2f(tile.x*tileSize, tile.y*tileSize));
    if(defeated) sp.setColor({255,255,255,120});
    win.draw(sp);
}
