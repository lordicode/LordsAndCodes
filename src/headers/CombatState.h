#pragma once
#include "State.h"
#include "Player.h"
#include "Enemy.h"
#include "MusicManager.h"
#include <SFML/Graphics.hpp>

class CombatState : public State {
private:
    Player& m_player;
    Enemy& m_enemy;
    MusicManager& m_music;
    
    sf::Font m_font;  
    sf::Text m_introText;  
    sf::Text m_inputText; 
    sf::RectangleShape m_inputBox;

public:
    CombatState(sf::RenderWindow& window, Player& player, Enemy& enemy, MusicManager& music);
    
    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void draw() override;
};