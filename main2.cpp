#include <SFML/Graphics.hpp>
#include "main.hpp"


int main()
{
    sf::RenderWindow window(sf::VideoMode(SCREEN_HEIGHT, SCREEN_WIDTH),"pong game" );
    sf::Clock clock;
 pong ball(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ball.get_shape());
  
        window.display();
    }

    return 0;
}