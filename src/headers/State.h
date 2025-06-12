#pragma once
#include <memory>
#include <SFML/Graphics.hpp>

class State {
protected:
    sf::RenderWindow& m_window;
    std::unique_ptr<State> m_nextState;

public:
    State(sf::RenderWindow& window)
    : m_window(window)
    {}

    virtual ~State() = default;

    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(sf::Time dt) = 0;
    virtual void draw() = 0;

    // Call this after update(); if non-null, it becomes the new active state
    std::unique_ptr<State> takeNextState() {
        return std::move(m_nextState);
    }
};
