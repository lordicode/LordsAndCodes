#pragma once

#include "State.h"
#include <SFML/Graphics.hpp>
#include <vector>

// Starter screen class
class TitleState : public State {
public:
    explicit TitleState(sf::RenderWindow& window);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void draw() override;

    // True once the user presses Enter on “Start Game.”
    bool isStartChosen() const { return m_startChosen; }

private:
    sf::Font               m_font;
    std::vector<sf::Text>  m_options;
    int                    m_selectedIndex = 0;
    bool                   m_startChosen = false;
};
