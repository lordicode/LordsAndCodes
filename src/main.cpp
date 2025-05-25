#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include <queue>
#include <vector>

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");
    window.setFramerateLimit(144);

    //const sf::Texture texture("\\Assets\\MapTiles\\2.png");
    //sf::Sprite house(texture);

    //house.setPosition({50.f, 30.f});

    while (window.isOpen())
    {    

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();
        //window.draw(house);
        window.display();       

    }
}
        // D:\\github\\LordsAndCodes\\Assets\\MapTiles\\2.png
    // TODO: enum class tiles, enum class game modes, assets, game loop, win and lose, tile effects, enemies, combat logic
