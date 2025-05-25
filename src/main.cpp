#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <map>
#include <queue>
#include <vector>

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");
    window.setFramerateLimit(144);

    //const sf::Texture texture("src\\Assets\\MapTiles\\2.png");
    //sf::Sprite house(texture);

    //house.setPosition({50.f, 30.f});
    //sf::Music music("src\\Assets\\Music\\tense.wav"); 
    //music.play();

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

    // TODO: enum class tiles, enum class game modes, assets, game loop, win and lose, tile effects, enemies, combat logic
