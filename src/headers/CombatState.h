#pragma once

#include "State.h"
#include "Player.h"
#include "Enemy.h"
#include "MusicManager.h"
#include "TextureManager.h"
#include "CombatChallenge.h"

#include <SFML/Graphics.hpp>
#include <string>
#include <deque>

class CombatState : public State {
public:
    CombatState(sf::RenderWindow& window,
                Player&           player,
                Enemy&            enemy,
                MusicManager&     music);

    ~CombatState() override; 

    void handleEvent(const sf::Event& event) override;
    void update    (sf::Time dt)             override;
    void draw      ()                        override;

    const std::string& getAnswer() const { return m_answer; }

private:
    // fixed UI
    void wrapAndPositionIntro();
    void positionPromptAndBox();
    void drawBars();                 // hp / armor sprites

    // challenge
    void pickNextChallenge();
    bool answerCorrect(const std::string&);

    // hit effect helpers
    void startHitSprite(bool enemySide);

    Player& m_player;
    Enemy& m_enemy;
    MusicManager&   m_music;
    std::string     m_prevMusicID;
    
    sf::Font  m_font;
    sf::Text  m_introText;
    sf::Text  m_promptText;
    sf::RectangleShape m_column;

    sf::RectangleShape m_inputBox;
    sf::Text  m_inputText;

    // textures for bars / hits
    sf::Texture m_hpTex;
    sf::Texture m_armorTex;
    sf::Texture m_hitTex;

    // sprites reused every frame
    sf::Sprite m_hitSprite;
    bool m_hitEnemy = true;
    float   m_hitDuration = 0.f;
    // user input
    std::u32string  m_userInput;
    std::string     m_answer;
    
    // challenges
    std::deque<const Challenge*> m_pool;  // current enemy’s challenges
    const Challenge*             m_curr   = nullptr;

    bool  m_uiReady = false;

    bool m_processed = false;
    bool m_wasCorrect = false;
    sf::Clock m_hitTimer;
};
