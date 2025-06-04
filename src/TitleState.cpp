#include "TitleState.h"
#include "MapViewState.h"
#include <iostream>

TitleState::TitleState(sf::RenderWindow& window)
: State(window)
{
    // SFML 3: load fonts using openFromFile or constructor
    if (!m_font.openFromFile("src/Assets/Fonts/ByteBounce.ttf")) {
        std::cerr << "Failed to open font at src/Assets/Fonts/ByteBounce.ttf\n";
    }

    // SFML 3: sf::Text constructor: (Font&, string, characterSize)
    sf::Text startText(m_font, "Start Game", 48);
    sf::Text optionsText(m_font,   "Options",    48);

    // Center horizontally; stack vertically
    sf::Vector2u winSize = m_window.getSize();
    startText.setPosition(
        sf::Vector2f(
            static_cast<float>(winSize.x / 2u - 100u),
            static_cast<float>(winSize.y / 2u -  50u)
        )
    );
    optionsText.setPosition(
        sf::Vector2f(
            static_cast<float>(winSize.x / 2u - 100u),
            static_cast<float>(winSize.y / 2u +  20u)
        )
    );

    m_options.push_back(startText);
    m_options.push_back(optionsText);

    // Highlight “Start Game” by default
    m_options[0].setFillColor(sf::Color::Yellow);
    m_options[1].setFillColor(sf::Color::White);
}

void TitleState::handleEvent(const sf::Event& event)
{
    // Close‐window event
    if (event.is<sf::Event::Closed>()) {
        m_window.close();
        return;
    }

    // Key‐pressed event
    if (event.is<sf::Event::KeyPressed>()) {
        auto keyEvt = event.getIf<sf::Event::KeyPressed>();
        if (!keyEvt) return; // should not happen

        switch (keyEvt->code)
        {
            case sf::Keyboard::Key::Up:
                if (m_selectedIndex > 0) {
                    m_options[m_selectedIndex].setFillColor(sf::Color::White);
                    --m_selectedIndex;
                    m_options[m_selectedIndex].setFillColor(sf::Color::Yellow);
                }
                break;

            case sf::Keyboard::Key::Down:
                if (m_selectedIndex < static_cast<int>(m_options.size()) - 1) {
                    m_options[m_selectedIndex].setFillColor(sf::Color::White);
                    ++m_selectedIndex;
                    m_options[m_selectedIndex].setFillColor(sf::Color::Yellow);
                }
                break;

            case sf::Keyboard::Key::Enter:
                if (m_selectedIndex == 0) {
                    // “Start Game” chosen → tell main loop to switch state
                    m_startChosen = true;
                }
                // If m_selectedIndex == 1, you could handle Options here.
                break;

            default:
                break;
        }
    }
}

void TitleState::update(sf::Time dt)
{
    (void)dt;
    // No animations or timers on title screen yet
}

void TitleState::draw()
{
    m_window.clear(sf::Color::Black);
    for (auto& txt : m_options) {
        m_window.draw(txt);
    }
    m_window.display();
}
