#include "CombatState.h"
#include "TextureManager.h"
#include <stdexcept>
#include <sstream>
#include <SFML/System/Utf.hpp>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <iostream>
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


CombatState::CombatState(sf::RenderWindow& window,
                         Player& p, Enemy& e, MusicManager& m)
  : State(window)
  , m_player(p)
  , m_enemy(e)
  , m_music(m)
  , m_introText(m_font, "", 28)
  , m_promptText(m_font, "Enter answer:", 28)
  , m_inputText(m_font, "", 26)
  , m_hitSprite(  
        TextureManager::getInstance()
        .getTexture("hit", "src/Assets/Effects/hit.png")
    )
{
    if (!m_font.openFromFile("src/Assets/Fonts/ByteBounce.ttf"))
        throw std::runtime_error("CombatState: failed to load font");
    m_prevMusicID = m_music.getCurrentMusicID();
    m_music.loadMusic("tense",   "D:\\github\\LordsAndCodes\\src\\Assets\\Music\\tense.wav");
    m_music.loadMusic("battle2", "D:\\github\\LordsAndCodes\\src\\Assets\\Music\\battle2.wav");
    m_music.loadMusic("battle",  "D:\\github\\LordsAndCodes\\src\\Assets\\Music\\battle.wav");
    std::vector<std::string> combat = { "tense", "battle2", "battle" };
    int i = std::rand() % combat.size();
    m_music.playMusic(combat[i], true);


    auto winSz = m_window.getSize();
    m_hitSprite.setOrigin(
        sf::Vector2f(
            m_hitSprite.getTexture().getSize()   
        ) * 0.5f
    );
    m_hitSprite.setScale({0.3, 0.3});
    if (!m_hpTex.loadFromFile("src/Assets/Effects/health.png"))
        throw std::runtime_error("health.png failed to load");

    if (!m_armorTex.loadFromFile("src/Assets/Effects/armor.png"))
        throw std::runtime_error("armor.png failed to load");

    // central column (35 % width, 55 % height)
    float colW = winSz.x * 0.35f;
    float colH = winSz.y * 0.55f;
    m_column.setSize( sf::Vector2f(colW, colH) );
    m_column.setFillColor(sf::Color(0,0,0,200));
    m_column.setPosition(
        sf::Vector2f((winSz.x - colW) * 0.5f,
                     (winSz.y - colH) * 0.25f)
    );

    const Challenge* ch = nullptr;
    if (!m_enemy.challenges.empty()) {
        int idx = std::rand() % static_cast<int>(m_enemy.challenges.size());
        ch = CombatChallenge::instance().get(m_enemy.challenges[idx]);
        m_curr = ch;  // store for answer checking
    }

    // wrap center both intro and challenge 
    colW      = m_column.getSize().x;
    float wrapWidth = colW - 20.f;
    unsigned int charSize = 20;

    // 1) wrap the intro
    std::string introWrapped = wrapText(
        m_font,
        m_enemy.getIntro(),
        charSize,
        wrapWidth
    );

    // 2) wrap the challenge prompt
    std::string challengeWrapped;
    if (ch) {
        challengeWrapped = wrapText(
            m_font,
            std::string("Challenge: ") + ch->prompt,
            charSize,
            wrapWidth
        );
    }

    // 3) concatenate
    std::string fullText = introWrapped;
    if (!challengeWrapped.empty())
        fullText += "\n\n" + challengeWrapped;

    // 4) apply & center
    m_introText.setCharacterSize(charSize);
    m_introText.setString(fullText);
    {
        sf::FloatRect b = m_introText.getLocalBounds();
        float cx = (colW - b.size.x) * 0.5f;
        m_introText.setPosition(
            m_column.getPosition() + sf::Vector2f(cx, 10.f)
        );
    }

    if (!m_enemy.challenges.empty()) {
       // choose one of this enemy’s challenge-IDs at random
       int idx = std::rand() % static_cast<int>(m_enemy.challenges.size());
       const Challenge* ch = CombatChallenge::instance().get(m_enemy.challenges[idx]);


    // recalc bounds and centre horizontally
    sf::FloatRect introB = m_introText.getLocalBounds();
    m_introText.setPosition(
        m_column.getPosition() +
        sf::Vector2f((m_column.getSize().x - introB.size.x) * 0.5f, 10.f)
    );
}

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
        if (key == sf::Keyboard::Key::Enter && !m_processed)
        {
            // 1) Turn the UTF-32 buffer into an std::string
            std::string utf8;
            sf::Utf32::toUtf8(m_userInput.begin(), m_userInput.end(),
                            std::back_inserter(utf8));

            // 2) Lowercase & trim
            auto ans = utf8;
            std::cout << ans << std::endl;
            std::transform(ans.begin(), ans.end(), ans.begin(),
                        [](unsigned char c){ return std::tolower(c); });

            // 3) Check against the current challenge’s answers
            m_wasCorrect = false;
            for (auto& valid : m_curr->answers) {
                if (ans == valid) {
                    m_wasCorrect = true;
                    break;
                }
            }

            // 4) Apply damage
            if (m_wasCorrect)
                m_enemy.takeDamage(1);
            else
                m_player.takeDamage(1);

            // 5) Start the hit animation
            m_processed = true;
            m_hitTimer.restart();
        }

    }
}

void CombatState::update(sf::Time)
{
    // once hit‐anim has playe
    if (m_processed && m_hitTimer.getElapsedTime().asSeconds() >= 0.5f) {
    // Game Over if player dies
    if (m_player.getHp() <= 0)
    {
        sf::Text gameOverText{ m_font, "GAME OVER", 72 };
        gameOverText.setFillColor(sf::Color::Red);

        auto winSz = m_window.getSize();
        auto bounds = gameOverText.getLocalBounds();
        float textW = bounds.size.x;  
        float textH = bounds.size.y;  
        gameOverText.setPosition({
            (winSz.x - textW) * 0.5f,
            (winSz.y - textH) * 0.5f
        });

        m_window.clear(sf::Color::Black);
        m_window.draw(gameOverText);
        m_window.display();
        std::this_thread::sleep_for(std::chrono::seconds(2));

        m_window.close();
        return;
    }
        if (m_enemy.getHp() <= 0) {
            // enemy died - signal to pop state
            m_answer = "enemy_defeated";
        }
        else
        {
            // pick a fresh random Challenge*
            int idx = std::rand() % m_enemy.challenges.size();
            const Challenge* ch =
                CombatChallenge::instance().get(m_enemy.challenges[idx]);
            m_curr = ch;

            // 1) pull out the old full string
            std::string full = m_introText.getString().toAnsiString();

            // 2) find the boundary where we appended the last challenge
            static const std::string marker = "\n\nChallenge:";
            auto pos = full.find(marker);

            // 3) if found, take only the intro; otherwise, assume full is just the intro
            std::string baseIntro = (pos != std::string::npos)
                ? full.substr(0, pos)
                : full;

            // 4) re-wrap the intro 
            // use the same font, char-size, and wrap-width as in the constructor
            float wrapWidth = m_column.getSize().x - 20.f; 
            unsigned int charSize = m_introText.getCharacterSize();

            // recalc postiiton of text
            sf::FloatRect introB = m_introText.getLocalBounds();
            float centeredX = (m_column.getSize().x - introB.size.x) * 0.5f;
            m_introText.setPosition(m_column.getPosition() + sf::Vector2f(centeredX, 10.f));
            
            m_answer.clear();
            m_processed = false;
            m_hitTimer.restart();
        }
    }
}

void CombatState::draw()
{
    // 1) Clear
    m_window.clear(sf::Color::Black);
    auto winSz = m_window.getSize();  

    //
    // 2) Player sprite + HP/armor icons
    //
    const auto& pTex = TextureManager::getInstance()
                          .getTexture(m_player.getSpriteFile(),
                                      "src/Assets/Player/" + m_player.getSpriteFile());
    sf::Sprite pSp{pTex};  
    sf::Vector2f pSize{ float(pTex.getSize().x),
                        float(pTex.getSize().y) };
    float baseScale = std::min(float(720) * 0.40f / pSize.y, 3.f);  // windowed reference (720p)
    float refHeight = 720.f;
    float sizeRatio = winSz.y / refHeight;
    float dampening = 1.f / std::pow(sizeRatio, 0.6f);  
    float scaleP = std::min((refHeight * 0.40f / pSize.y) * dampening, 3.f);
    pSp.setScale({ scaleP, scaleP });         
    float playerY = (float(winSz.y) - pSize.y * scaleP) * 0.5f;
    playerY = std::max(playerY, 30.f);  // avoid going off the bottom
    pSp.setPosition({ 50.f, playerY });
    m_window.draw(pSp);

    // draw player HP & armor
    sf::Sprite hpIcon{m_hpTex}, arIcon{m_armorTex};
    hpIcon.setScale({0.5f, 0.5f});
    arIcon.setScale({0.5f, 0.5f});
    float iconW = hpIcon.getGlobalBounds().size.x, gap = 2.f;
    for (int i = 0; i < m_player.getHp(); ++i) {
        hpIcon.setPosition({ 50.f + i * (iconW + gap), 30.f });
        m_window.draw(hpIcon);
    }
    for (int i = 0; i < m_player.getArmor(); ++i) {
        arIcon.setPosition({ 50.f + (m_player.getHp() + i) * (iconW + gap),
                             30.f });
        m_window.draw(arIcon);
    }
    // enemy HP on right
    sf::Sprite ehpIcon{m_hpTex};
    ehpIcon.setScale({0.5f, 0.5f});
    float eIconW = ehpIcon.getGlobalBounds().size.x;
    float totalW = m_enemy.getHp() * eIconW + (m_enemy.getHp() - 1) * gap;
    float startX = float(winSz.x) - 50.f - totalW;
    for (int i = 0; i < m_enemy.getHp(); ++i) {
        ehpIcon.setPosition({ startX + i * (eIconW + gap), 30.f });
        m_window.draw(ehpIcon);
    }

    //
    // 3) Enemy sprite
    //
    const auto& eTex = TextureManager::getInstance()
                          .getTexture(m_enemy.getSpriteFile(),
                                      "src/Assets/Enemy/" + m_enemy.getSpriteFile());
    sf::Sprite eSp{eTex};  
    sf::Vector2f eSize{ float(eTex.getSize().x),
                        float(eTex.getSize().y) };
    float scaleE = float(winSz.y) * 0.40f / eSize.y;
    eSp.setScale({ scaleE, scaleE }); 
    eSp.setPosition({
        float(winSz.x) - eSp.getGlobalBounds().size.x - 50.f,
        std::max((float(winSz.y) - eSize.y * scaleE) * 0.5f, 30.f)
    });
    m_window.draw(eSp);

    //
    // 4) Hit animation
    //
    if (m_processed && m_hitTimer.getElapsedTime().asSeconds() < 0.5f)
    {
        sf::Sprite hit{m_hitSprite};  // Sprite(const Texture&) only:contentReference[oaicite:5]{index=5}
        sf::Vector2f center;
        if (m_wasCorrect) {
            center = eSp.getPosition()
                   + sf::Vector2f(eSize.x * scaleE * 0.5f,
                                  eSize.y * scaleE * 0.5f);
        } else {
            center = pSp.getPosition()
                   + sf::Vector2f(pSize.x * scaleP * 0.5f,
                                  pSize.y * scaleP * 0.5f);
        }
        hit.setPosition(center);  
        m_window.draw(hit);
    }

    //
    // 5) UI overlay
    //
    m_window.draw(m_column);
    m_window.draw(m_introText);
    m_window.draw(m_promptText);
    m_window.draw(m_inputBox);
    m_window.draw(m_inputText);

    m_window.display();
    m_uiReady = true;
}

CombatState::~CombatState()
{
    // as soon as this state goes away, kick the map music back in
    m_music.playMusic(m_prevMusicID, false);
}