#include "CombatState.h"
#include "TextureManager.h"

#include <stdexcept>
#include <sstream>
#include <SFML/System/Utf.hpp>


// wrap text
static std::string wrapText(const sf::Font& font,
                            const std::string& in,
                            unsigned int size,
                            float maxPixels)
{
    std::istringstream stream(in);
    std::string word, line, out;

    while (stream >> word)
    {
        std::string test = line.empty() ? word : line + " " + word;
        sf::Text txt(font, test, size);
        if (txt.getLocalBounds().size.x > maxPixels)
        {
            if (!out.empty()) out += '\n';
            out += line;
            line = word;
        }
        else line = std::move(test);
    }
    if (!line.empty())
    {
        if (!out.empty()) out += '\n';
        out += line;
    }
    return out;
}
// ─────────────────────────────────────────────────────────────

CombatState::CombatState(sf::RenderWindow& window,
                         Player& player,
                         Enemy& enemy,
                         MusicManager& music)
  : State(window)
  , m_player(player)
  , m_enemy(enemy)
  , m_music(music)
  , m_introText (m_font, "",             40)          
  , m_promptText(m_font, "Enter answer:", 32)
  , m_inputText (m_font, "",              32)
{
    if (!m_font.openFromFile("src/Assets/Fonts/ByteBounce.ttf"))
        throw std::runtime_error("CombatState: failed to load font");

    auto winSz = m_window.getSize();

    // central column (35 % width, 55 % height)
    float colW = winSz.x * 0.35f;
    float colH = winSz.y * 0.55f;
    m_column.setSize( sf::Vector2f(colW, colH) );
    m_column.setFillColor(sf::Color(0,0,0,200));
    m_column.setPosition(
        sf::Vector2f((winSz.x - colW) * 0.5f,
                     (winSz.y - colH) * 0.25f)
    );

    // wrapped intro
    std::string wrapped = wrapText(m_font, enemy.getIntro(), 20, colW - 20.f);
    m_introText.setString(wrapped);
    m_introText.setFillColor(sf::Color::White);
    sf::FloatRect introB = m_introText.getLocalBounds();
    m_introText.setPosition(
        sf::Vector2f(
            m_column.getPosition().x + (colW - introB.size.x) * 0.5f,
            m_column.getPosition().y + 10.f
        )
    );

    // prompt
    m_promptText.setFillColor(sf::Color::White);
    sf::FloatRect promptBounds = m_promptText.getLocalBounds();

    float promptX = (winSz.x - promptBounds.size.x) * 0.5f;
    float promptY = m_column.getPosition().y + m_column.getSize().y + 30.f;  // 20-px gap below column
    m_promptText.setPosition(sf::Vector2f(promptX, promptY));

    // input box 
    float boxY = promptY + promptBounds.size.y + 38.f;         // 8-px gap below prompt
    m_inputBox.setSize(sf::Vector2f(m_column.getSize().x * 0.8f, 30.f));
    m_inputBox.setFillColor(sf::Color::White);
    m_inputBox.setPosition(sf::Vector2f(
        (winSz.x - m_inputBox.getSize().x) * 0.5f,
        boxY
    ));

    // input text 
    m_inputText.setCharacterSize(16);
    m_inputText.setFillColor(sf::Color::Black);
    m_inputText.setPosition(m_inputBox.getPosition() + sf::Vector2f(4.f, 4.f));
}

void CombatState::handleEvent(const sf::Event& event)
{
    if (!m_uiReady) return;

    if (event.is<sf::Event::TextEntered>())
    {
        char32_t c = event.getIf<sf::Event::TextEntered>()->unicode;
        if (c == 8 && !m_userInput.empty())
            m_userInput.pop_back();
        else if (c >= 32 && c < 127)
            m_userInput += c;

        std::string utf8;
        sf::Utf32::toUtf8(m_userInput.begin(), m_userInput.end(),
                        std::back_inserter(utf8));

        // measure width using a temporary sf::Text
        sf::Text test(m_font, utf8, m_inputText.getCharacterSize());
        if (test.getLocalBounds().size.x > m_inputBox.getSize().x - 8.f)
            return;                         // ignore char that would overflow

        m_inputText.setString(utf8);
    }


    if (event.is<sf::Event::KeyPressed>())
    {
        auto key = event.getIf<sf::Event::KeyPressed>()->code;
        if (key == sf::Keyboard::Key::Enter)
        {
            std::string utf8;
            sf::Utf32::toUtf8(m_userInput.begin(), m_userInput.end(),
                              std::back_inserter(utf8));
            m_answer = utf8;   // MapViewState / main() will pop this state
        }
    }
}

void CombatState::update(sf::Time) {}

void CombatState::draw()
{
    m_window.clear(sf::Color::Black);
    auto winSz = m_window.getSize();

    {
        const std::string& key = m_player.getSpriteFile();
        const auto& tx = TextureManager::getInstance()
                           .getTexture(key, "src/Assets/Player/" + key);

        sf::Sprite sp{tx};
        auto ts = tx.getSize();
        float scale = float(winSz.y) * 0.40f / float(ts.y);

        sp.setScale(sf::Vector2f(scale, scale));
        sp.setPosition(sf::Vector2f(50.f, (winSz.y - ts.y * scale) * 0.5f));
        m_window.draw(sp);

        // HP
        for (int i = 0; i < m_player.getHp(); ++i) {
            sf::RectangleShape bar(sf::Vector2f(20.f, 5.f));
            bar.setFillColor(sf::Color::Green);
            bar.setPosition(sf::Vector2f(50.f + i * 22.f, 30.f));
            m_window.draw(bar);
        }
        // Armor
        for (int i = 0; i < m_player.getArmor(); ++i) {
            sf::RectangleShape bar(sf::Vector2f(20.f, 5.f));
            bar.setFillColor(sf::Color::Blue);
            bar.setPosition(sf::Vector2f(50.f + (m_player.getHp() + i) * 22.f, 30.f));
            m_window.draw(bar);
        }
    }

    {
        const std::string& key = m_enemy.getSpriteFile();
        const auto& tx = TextureManager::getInstance()
                           .getTexture(key, "src/Assets/Enemy/" + key);

        sf::Sprite sp{tx};
        auto ts = tx.getSize();
        float scale = float(winSz.y) * 0.40f / float(ts.y);

        sp.setScale(sf::Vector2f(scale, scale));
        sp.setPosition(sf::Vector2f(
            winSz.x - ts.x * scale - 50.f,
            (winSz.y - ts.y * scale) * 0.5f));
        m_window.draw(sp);

        for (int i = 0; i < m_enemy.getHp(); ++i) {
            sf::RectangleShape bar(sf::Vector2f(20.f, 5.f));
            bar.setFillColor(sf::Color::Green);
            bar.setPosition(sf::Vector2f(
                winSz.x - ts.x * scale - 50.f + i * 22.f, 30.f));
            m_window.draw(bar);
        }
    }

    m_window.draw(m_column);
    m_window.draw(m_introText);

    m_window.draw(m_promptText);
    m_window.draw(m_inputBox);
    m_window.draw(m_inputText);

    m_window.display();
    m_uiReady = true;
}
