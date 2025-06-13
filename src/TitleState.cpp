#include "TitleState.h"
#include <iostream>

TitleState::TitleState(sf::RenderWindow& window, MusicManager& music)
  : State(window)
  , m_music(music)
{
    if (!m_font.openFromFile("src/Assets/Fonts/ByteBounce.ttf")) {
        std::cerr << "TitleState: Failed to load font\n";
    }

    // Create two menu items
    sf::Text startText   { m_font, "Start Game",  58 };
    sf::Text optionsText { m_font, "",     48 };

    // Center them on screen
    auto winSz = m_window.getSize();
    float centerX = winSz.x * 0.5f;
    float centerY = winSz.y * 0.5f;

    auto startBounds   = startText  .getLocalBounds();
    auto optionsBounds = optionsText.getLocalBounds();

    startText.setPosition({
        centerX - startBounds.size.x * 0.5f,
        centerY - 50.f
    });
    optionsText.setPosition({
        centerX - optionsBounds.size.x * 0.5f,
        centerY + 20.f
    });

    m_options.push_back(startText);
    m_options.push_back(optionsText);

    m_options[0].setFillColor(sf::Color::Yellow);
    m_options[1].setFillColor(sf::Color::White);
}

void TitleState::handleEvent(const sf::Event& event)
{
    if (event.is<sf::Event::Closed>()) {
        m_window.close();
        return;
    }

    if (event.is<sf::Event::KeyPressed>()) {
        auto code = event.getIf<sf::Event::KeyPressed>()->code;

        if (code == sf::Keyboard::Key::Up) {
            if (m_selectedIndex > 0) {
                m_options[m_selectedIndex].setFillColor(sf::Color::White);
                --m_selectedIndex;
                m_options[m_selectedIndex].setFillColor(sf::Color::Yellow);
            }
        }
        else if (code == sf::Keyboard::Key::Down) {
            if (m_selectedIndex + 1 < static_cast<int>(m_options.size())) {
                m_options[m_selectedIndex].setFillColor(sf::Color::White);
                ++m_selectedIndex;
                m_options[m_selectedIndex].setFillColor(sf::Color::Yellow);
            }
        }
        else if (code == sf::Keyboard::Key::Enter) {
            if (m_selectedIndex == 0)
                m_startChosen = true;
        }
    }
}

void TitleState::update(sf::Time)
{
    // nothing for now
}

void TitleState::draw()
{
    m_window.clear(sf::Color::Black);
    for (auto& txt : m_options)
        m_window.draw(txt);
    m_window.display();
}