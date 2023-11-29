#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <algorithm>
#include "global.hpp"

void movePaddle(sf::RectangleShape& paddle1, sf::RectangleShape& paddle2, float deltaTime){
    const float paddleSpeed = 450.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
        float nextPosition = paddle1.getPosition().y;
        nextPosition -= paddleSpeed * deltaTime;
        if (nextPosition > 0)
            paddle1.setPosition(0, nextPosition);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
        float nextPosition = paddle1.getPosition().y;
        nextPosition += paddleSpeed * deltaTime;
        if (nextPosition < 450)
            paddle1.setPosition(0, nextPosition);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        float nextPosition = paddle2.getPosition().y;
        nextPosition -= paddleSpeed * deltaTime;
        if (nextPosition > 0)
            paddle2.setPosition(1180, nextPosition);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        float nextPosition = paddle2.getPosition().y;
        nextPosition += paddleSpeed * deltaTime;
        if (nextPosition < 450)
            paddle2.setPosition(1180, nextPosition);
    }
}

//totally made my code more readable by making this one function separate
bool isCollision(sf::RectangleShape& ball, sf::RectangleShape& paddle) {
    return ball.getGlobalBounds().intersects(paddle.getGlobalBounds());
}

sf::Vector2f moveBall(sf::RectangleShape& ball, float deltaTime, sf::Vector2f direction, 
                        sf::RectangleShape& paddle1, sf::RectangleShape& paddle2, int* score1, int* score2){
    float speed = 800.0f;
    
    //handling collision with the paddle by negating the x component of the vector (180 degree turn) and giving y direction a random value
    if (isCollision(ball, paddle1) || isCollision(ball, paddle2)){
        direction.x = -direction.x;
        direction.y = -0.9 + static_cast<float>(std::rand()) / RAND_MAX * (0.9 + 0.9);

        //set the ball location to (1+end of the paddle, current y position)
        if (isCollision(ball, paddle1)){
            ball.setPosition(21, ball.getPosition().y);
        }
        if (isCollision(ball, paddle2)){
            ball.setPosition(1159, ball.getPosition().y);
        }
    }

    //handling collision with the top and bottom of the screen
    if (ball.getPosition().y < 1){
        direction.y = -direction.y;
        ball.setPosition(ball.getPosition().x, 1); // Adjust position to be inside screen bounds
    }
    else if (ball.getPosition().y > 580){
        direction.y = -direction.y;
        ball.setPosition(ball.getPosition().x, 580); // Adjust position to be inside screen bounds
    }

    //calculate the next position of the ball, deltaTime for making it framerate independent
    sf::Vector2f nextPosition = ball.getPosition() + (speed * direction * deltaTime);

    //set the current position to the calculated next position if the ball is inside the screen
    if (nextPosition.x > 0 && nextPosition.x < 1200 && nextPosition.y > 0 && nextPosition.y < 600){
        ball.setPosition(nextPosition);
    }

    //if the ball is offscreen, reverse its x direction (so it flies towards the player who scored)
    //and reset the paddles and ball to their initial positions
    if (nextPosition.x < 5 || nextPosition.x > 1195) {
        if (nextPosition.x < 5)
            (*score2)++;
        else
            (*score1)++;
        direction.x = - direction.x;
        direction.y = 0;
        ball.setPosition(580,290);
    }

    return direction;
}

void computerPlayer(sf::RectangleShape& ball, sf::RectangleShape& paddle1, float deltaTime){
    float paddle_speed = 450.0f;

    //predict the ball's position and adjust the paddle's position accordingly
    float targetY = ball.getPosition().y;

    //if ball is in the opponent half of the screen, add/subtract from targetY accordingly 
    if (ball.getPosition().x > SCREEN_WIDTH / 2) {
        targetY += (ball.getPosition().y > paddle1.getPosition().y) ? paddle_speed * deltaTime : -paddle_speed * deltaTime;
    }

    targetY = std::max(0.0f, std::min(450.0f, targetY));

    float currentY = paddle1.getPosition().y;
    float direction = (targetY > currentY) ? 1.0f : -1.0f;
    float nextPosition = currentY + direction * paddle_speed * deltaTime;

    paddle1.setPosition(0, nextPosition);
}

void handleStartInput(bool* computer, bool* startUpChoice, sf::RenderWindow& window, sf::Font& font){
    sf::Text startText("Choose Game Mode: \n Single Player (Press Up Arrow) \n Two Player (Press Down arrow)", font, 70);
    startText.setFillColor(sf::Color::White);
    startText.setPosition(100,100);
    window.draw(startText);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        *startUpChoice = 1;
        *computer = 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        *startUpChoice = 1;
    }
}

void handleGameOver(bool& gameOver, sf::RenderWindow& window, sf::Font& font, int& score1, int& score2, bool computer) {
    sf::Text endText("Game Over!", font, 100);
    endText.setFillColor(sf::Color::White);
    endText.setPosition(400, 100);

    sf::Text scoreText("", font, 70);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(410, 200);

    if (score1 >= 10) {
        if (computer) {
            scoreText.setString("Computer wins");
        } else {
            scoreText.setString(" Player1 wins");
        }
        
    }
    if (score2 >= 10) {
        scoreText.setString(" Player2 wins");
    }

    sf::Text replayText("Press R to Play Again \n  Press esc to Quit", font, 50);
    replayText.setFillColor(sf::Color::White);
    replayText.setPosition(410,400);

    while (window.isOpen() && gameOver) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                gameOver = false; 
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
            gameOver = 0;
            score1 = 0;
            score2 = 0;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
        }

        window.clear();
        window.draw(endText);
        window.draw(scoreText);
        window.draw(replayText);
        window.display();
    }
}

int main()
{
    int scorePaddle1 = 0, scorePaddle2 = 0;
    bool computer = 0, gameOver = 0, startUpChoice = 0; 

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Pong");

    //Left paddle
    sf::RectangleShape paddle1(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
    paddle1.setPosition(0, 225);

    //Right paddle
    sf::RectangleShape paddle2(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
    paddle2.setPosition(1180, 225);

    //Ball(?)
    sf::RectangleShape ball(sf::Vector2f(BALL_SIDE, BALL_SIDE));
    ball.setPosition(590, 290);

    //Load font
    sf::Font font;
    if (!font.loadFromFile("C:\\Users\\devan\\Downloads\\VT323\\pixelfont.ttf")) {
        return EXIT_FAILURE;
    }
    
    //used to calculate the time between frames
    sf::Clock clock;

    //intializes the direction vector, ball travels to the right when the game starts
    sf::Vector2f direction = {1.0f, 0.0f}; 

    while (window.isOpen())
    {
        sf::Event event;

        //check if window closed
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
        }

        //game not started yet
        if (startUpChoice == 0){
            //will flip startUpChoice to 1 and also provide input on if the user wants game to be single player or two player
            handleStartInput(&computer, &startUpChoice, window, font);
        }

        if (startUpChoice == 1 && gameOver == 0){
            float deltaTime = clock.restart().asSeconds();

            //didnt want to define an overload so you can still move paddle1 when computerplayer is on (wont really work)
            movePaddle(paddle1, paddle2, deltaTime);

            if (computer == 1){
                computerPlayer(ball, paddle1, deltaTime);
            }
                        
            std::string scoreText1 = std::to_string(scorePaddle1);
            std::string scoreText2 = std::to_string(scorePaddle2);

            direction = moveBall(ball, deltaTime, direction, paddle1, paddle2, &scorePaddle1, &scorePaddle2);

            if (scorePaddle1 >= 10 || scorePaddle2 >= 10){
                gameOver = 1;
                handleGameOver(gameOver, window, font, scorePaddle1, scorePaddle2, computer);
            }

            sf::Text Score1(scoreText1, font, 100);
            Score1.setFillColor(sf::Color::White);
            Score1.setPosition(500, 30);

            sf::Text Score2(scoreText2, font, 100);
            Score2.setFillColor(sf::Color::White);
            Score2.setPosition(650,30);

            window.clear();
            window.draw(paddle1);
            window.draw(paddle2);
            window.draw(ball);
            window.draw(Score1);
            window.draw(Score2);

            //column of squares
            for (int i = 0; i < 19; i++){
                sf::RectangleShape square(sf::Vector2f(BALL_SIDE,BALL_SIDE));
                square.setPosition(SCREEN_WIDTH/2 - 10, BALL_SIDE + BALL_SIDE * 2*i);
                window.draw(square);
            }
        }
        window.display();
    }

    return 0;
}
