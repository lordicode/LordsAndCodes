#pragma once

#include <SFML/Graphics.hpp>

// class for any “screen” or state, as per professor suggestion, will be used to exchange information between states
class State {
public:
    explicit State(sf::RenderWindow& window)
    : m_window(window)
    {}

    virtual ~State() = default;

    virtual void handleEvent(const sf::Event& event) = 0;

    virtual void update(sf::Time dt) = 0;

    virtual void draw() = 0;

protected:
    sf::RenderWindow& m_window;
};
