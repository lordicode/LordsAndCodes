#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <ctime>
#include <memory>
#include <vector>
#include <stack>
#include "TitleState.h"
#include "MapViewState.h"
#include "CombatState.h"
#include "MusicManager.h"

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Single shared music manager, previous solution was crashing when transferring between stages
    MusicManager music;

    // Create window
    sf::RenderWindow window(sf::VideoMode{ sf::Vector2u{1200u,700u} }, "Lord & Codes");
    window.setFramerateLimit(144);

    sf::Clock clock;

    // State stack
    std::vector<std::unique_ptr<State>> states;
    states.emplace_back(std::make_unique<TitleState>(window, music));

    while (window.isOpen() && !states.empty())
    {
        sf::Time dt = clock.restart();

        // Poll events for the top state only
        while (auto evOpt = window.pollEvent())
        {
            const sf::Event& event = *evOpt;
            if (event.is<sf::Event::Closed>())
                window.close();

            states.back()->handleEvent(event);
        }

        // Update top
        states.back()->update(dt);

        // Check if top produced a nextState
        if (auto next = states.back()->takeNextState())
        {
            // If we're in MapViewState and next is CombatState
            if (dynamic_cast<MapViewState*>(states.back().get())
                && dynamic_cast<CombatState*>(next.get()))
            {
                states.emplace_back(std::move(next));
            }
            else
            {
                states.back() = std::move(next);
            }
            continue; 
        }

        // Draw top state
        states.back()->draw();

        if (auto combat = dynamic_cast<CombatState*>(states.back().get()))
        {
            if (!combat->getAnswer().empty())
            {
                states.pop_back();
                continue;
            }
        }

        // MapView transition 
        if (auto title = dynamic_cast<TitleState*>(states.back().get()))
        {
            if (title->isStartChosen())
            {
                states.back() = std::make_unique<MapViewState>(window, music);
            }
        }
    }

    return 0;
}
