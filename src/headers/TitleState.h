#pragma once

#include "State.h"
#include "MusicManager.h"
#include <SFML/Graphics.hpp>

class TitleState : public State {
public:
    TitleState(sf::RenderWindow& window, MusicManager& music);

    void handleEvent(const sf::Event& event) override;
    void update    (sf::Time dt)         override;
    void draw      ()                    override;

    bool isStartChosen() const { return m_startChosen; }

private:
    MusicManager&   m_music;          // reference to the shared manager
    sf::Font               m_font;
    std::vector<sf::Text>  m_options;
    int                    m_selectedIndex = 0;
    bool                   m_startChosen = false;
};
