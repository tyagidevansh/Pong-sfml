#pragma once
#include <SFML/Graphics.hpp>

float BALL_SIDE = 20;

int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 600;
int PADDLE_WIDTH = 20;
int PADDLE_HEIGHT = 150;
float directionx = 1.0f;
float directiony = 1.0f;
float SPEED=200;
class pong{
    sf::Vector2f movement;
    sf::Vector2f Position;
    sf::RectangleShape ball;
    


public:


pong(float x,float y);

sf::RectangleShape get_shape();

int getballshape();

sf::Vector2f moveball();
sf::FloatRect get_position();

void update(sf::Time dt);


};