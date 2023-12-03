#include "main.hpp"






pong::pong(float x, float y){
Position.x= x;
Position.y= y;

ball.setSize(sf::Vector2f(BALL_SIDE,BALL_SIDE));
ball.setPosition(Position);
}


sf::RectangleShape pong::get_shape(){
    return ball;
}

sf::FloatRect pong::get_position(){
   Position.x=SCREEN_WIDTH/2;
   Position.y=SCREEN_HEIGHT/2;
}

sf::Vector2f pong::moveball(){
     Position.x+= directionx;
     Position.y+=directiony;
}

void pong::update(sf::Time dt){
Position.x += directionx*SPEED*dt.asSeconds();
Position.y += directiony*SPEED*dt.asSeconds();

get_shape().setPosition(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
}