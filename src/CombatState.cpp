#include "CombatState.h"
#include "Player.h"
#include "Enemy.h"
#include "TextureManager.h"
#include "MapViewState.h" 
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <iostream>

// CombatState::CombatState(sf::RenderWindow& window, Player& player, Enemy& enemy, MusicManager& music) 
//     : State(window), m_player(player), m_enemy(enemy), m_music(music) {
// }

// void CombatState::handleEvent(const sf::Event& event) {

// }

// void CombatState::update(sf::Time dt) {
//     // No updates to stop animations
// }

// void CombatState::draw() {
//     m_window.clear(sf::Color::Black);
//     m_window.draw(m_introText);
//     m_window.draw(m_inputBox);
//     m_window.draw(m_inputText);
//     m_window.display();
// }