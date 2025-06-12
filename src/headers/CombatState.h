#pragma once

#include "State.h"
#include "Player.h"
#include "Enemy.h"
#include "MusicManager.h"
#include "TextureManager.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <string>

class CombatState : public State {
public:
    CombatState(sf::RenderWindow& window,
                Player& player,
                Enemy& enemy,
                MusicManager& music);

    void handleEvent(const sf::Event& event) override;
    void update    (sf::Time dt)         override;
    void draw      ()                    override;

    const std::string& getAnswer() const { return m_answer; }

private:
    Player&         m_player;
    Enemy&          m_enemy;
    MusicManager&   m_music;       // reference into the shared manager

    sf::Font        m_font;
    sf::Text        m_introText;   // initialized in ctor
    sf::Text        m_promptText;
    sf::RectangleShape m_column;

    sf::RectangleShape m_inputBox;
    sf::Text        m_inputText;   // initialized in ctor

    std::u32string  m_userInput;   // for composing UTF-32 input
    std::string     m_answer;

    bool            m_uiReady = false;
};
