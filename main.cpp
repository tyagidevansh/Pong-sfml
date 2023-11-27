#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>

void movePaddle(sf::RectangleShape& paddle1, sf::RectangleShape& paddle2, float deltaTime){
    const float paddleSpeed = 250.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        float nextPosition = paddle1.getPosition().y;
        nextPosition -= paddleSpeed * deltaTime;
        if (nextPosition > 0)
            paddle1.setPosition(0, nextPosition);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        float nextPosition = paddle1.getPosition().y;
        nextPosition += paddleSpeed * deltaTime;
        if (nextPosition < 450)
            paddle1.setPosition(0, nextPosition);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
        float nextPosition = paddle2.getPosition().y;
        nextPosition -= paddleSpeed * deltaTime;
        if (nextPosition > 0)
            paddle2.setPosition(1180, nextPosition);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
        float nextPosition = paddle2.getPosition().y;
        nextPosition += paddleSpeed * deltaTime;
        if (nextPosition < 450)
            paddle2.setPosition(1180, nextPosition);
    }
}

bool isCollision(sf::RectangleShape& ball, sf::RectangleShape& paddle) {
    sf::FloatRect ballBox = ball.getGlobalBounds();
    sf::FloatRect paddleBox = paddle.getGlobalBounds();
    if (ballBox.intersects(paddleBox)){
        return true;
    }
    return false;
}

sf::Vector2f moveBall(sf::RectangleShape& ball, float deltaTime, sf::Vector2f direction, sf::RectangleShape& paddle1, sf::RectangleShape& paddle2){
    float speed = 700.0f;
    
    //handling collision with the paddle by negating the x component of the vector (180 degree turn) and giving y direction a random value
    if (isCollision(ball, paddle1) || isCollision(ball, paddle2)){
        direction.x = -direction.x;
        direction.y = -0.9 + static_cast<float>(std::rand()) / RAND_MAX * (0.9 + 0.9);
    }

    //handling collision with the top and bottom of the screen
    if (ball.getPosition().y < 5 || ball.getPosition().y > 580){
        printf("Off the screen");
        direction.y = -direction.y;
    }

    sf::Vector2f nextPosition = ball.getPosition() + (speed * direction * deltaTime);

    if (nextPosition.x > 0 && nextPosition.x < 1200 && nextPosition.y > 0 && nextPosition.y < 600){
        ball.setPosition(nextPosition);
    }

    if (nextPosition.x < 5 || nextPosition.x > 1195) {
        direction.x = - direction.x;
        direction.y = 0;
        ball.setPosition(580,290);
    }

    return direction;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 600), "Pong");

    //Left paddle
    sf::RectangleShape paddle1(sf::Vector2f(20, 150));
    paddle1.setFillColor(sf::Color::White);
    paddle1.setPosition(0, 225);

    //Right paddle
    sf::RectangleShape paddle2(sf::Vector2f(20, 150));
    paddle2.setFillColor(sf::Color::White);
    paddle2.setPosition(1180, 225);

    //Ball(?)
    sf::RectangleShape ball(sf::Vector2f(20,20));
    ball.setFillColor(sf::Color::White);
    ball.setPosition(590, 290);

    //column of squares
    
    sf::Clock clock;
    sf::Vector2f direction = {-1.0f, 0.0f}; 

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
        }

        float deltaTime = clock.restart().asSeconds();
        movePaddle(paddle1, paddle2, deltaTime);

        direction = moveBall(ball, deltaTime, direction, paddle1, paddle2);


        window.clear();
        window.draw(paddle1);
        window.draw(paddle2);
        window.draw(ball);
        window.display();
    }

    return 0;
}
