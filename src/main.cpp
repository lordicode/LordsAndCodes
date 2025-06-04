#include <SFML/Graphics.hpp>
#include <memory>
#include "State.h"
#include "TitleState.h"
#include "MapViewState.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({1200, 700}), "Lord & Codes");
    window.setFramerateLimit(144);

    // Start on the Title screen.
    std::unique_ptr<State> currentState = std::make_unique<TitleState>(window);

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Time dt = clock.restart();

        while (auto maybeEvent = window.pollEvent())
        {
            const sf::Event& event = *maybeEvent;
            currentState->handleEvent(event);
        }

        currentState->update(dt);
        currentState->draw();

        // If we’re still in TitleState and “Start Game” was chosen, swap to MapViewState:
        if (auto titlePtr = dynamic_cast<TitleState*>(currentState.get())) {
            if (titlePtr->isStartChosen()) {
                currentState = std::make_unique<MapViewState>(window);
            }
        }
    }

    return 0;
}
