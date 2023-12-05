#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <algorithm>

int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 600;
int PADDLE_WIDTH = 20;
int PADDLE_HEIGHT = 150;
int BALL_SIDE = 20;

class Paddle {
private:
    sf::RectangleShape shape;

public:
    //constructor
    Paddle(float x, float y, float width, float height) : shape(sf::Vector2f(width, height)) {
        shape.setPosition(x, y);
    }

    void move(float deltaY) {
        float nextPosition = shape.getPosition().y + deltaY;
        //set the current position to the calculated next position if paddle is inside the screen
        if (nextPosition >= 0 && nextPosition <= SCREEN_HEIGHT - shape.getSize().y)
            shape.setPosition(shape.getPosition().x, nextPosition);
    }

    //getter
    const sf::RectangleShape& getShape() const {
        return shape;
    }
};

class Ball {
private:
    sf::RectangleShape shape;
public:
    //constructor
    Ball(float x, float y, float side) : shape(sf::Vector2f(side, side)) {
        shape.setPosition(x, y);
    }

    void move(sf::Vector2f delta) {
        shape.setPosition(shape.getPosition() + delta);
    }

    const sf::RectangleShape& getShape() const {
        return shape;
    }

};

class PongGame {
public:
    //sets the initial values of both the paddles and the ball
    PongGame() : paddle1(0, 225, PADDLE_WIDTH, PADDLE_HEIGHT), paddle2(1180, 225, PADDLE_WIDTH, PADDLE_HEIGHT), ball(590, 290, BALL_SIDE) {
        direction = {1.0f, 0.0f}; //the ball flies to the right when the game starts
        scorePaddle1 = 0; //scores are initially 0
        scorePaddle2 = 0;
        computer = false; //depends on choice made on front screen
        gameOver = false; //only true when one player reaches 10 points
        startUpChoice = false; //has the game mode choice been made?
    }

    void handleInput(sf::RenderWindow& window, sf::Font& font) {
        //sfml text class for rendering text
        sf::Text startText("Choose Game Mode: \n Single Player (Press Up Arrow) \n Two Player (Press Down arrow)", font, 70);
        startText.setFillColor(sf::Color::White);
        startText.setPosition(100,100);
        window.draw(startText);

        //we use keyboard input to determine made mode choice and to know if game has started
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
            startUpChoice = true;
            computer = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
            startUpChoice = true;
        }
    }

    void handleGameOver(sf::RenderWindow& window, sf::Font& font) {
        sf::Text endText("Game Over!", font, 100);
        endText.setFillColor(sf::Color::White);
        endText.setPosition(400, 100);

        //score text is initially empty
        sf::Text scoreText("", font, 70);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(410, 200);

        //we set what the end screen text should display on the basis of who won
        if (scorePaddle1 >= 10) {
            if (computer) {
                scoreText.setString("Computer wins");
            } else {
                scoreText.setString("Player 1 wins");
            }
        } else if (scorePaddle2 >= 10) {
            scoreText.setString("Player 2 wins");
        }
        sf::Text replayText("Press R to Play Again \n  Press esc to Quit", font, 50);
        replayText.setFillColor(sf::Color::White);
        replayText.setPosition(410,400);

        while (window.isOpen() && gameOver) {
            //checking if window needs to be closed
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    gameOver = false; 
                }
            }

            //if R (replay) is pressed, we reset the scores and gameOver states
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
                gameOver = false; //now the main game loop and continue executing
                scorePaddle1 = 0;
                scorePaddle2 = 0;
            }

            //close the wuindow if escape key pressed
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
                window.close();
            }

            //clear and redraw all the elements on screen
            window.clear();
            window.draw(endText);
            window.draw(scoreText);
            window.draw(replayText);
            window.display();
        }
    }

    //function to update the game every frame, including paddles, ball and text
    void update(float deltaTime) {
        if (!startUpChoice)
            return;

        movePaddle(paddle1, paddle2, deltaTime);

        if (computer)
            computerPlayer(deltaTime);

        std::string scoreText1 = std::to_string(scorePaddle1);
        std::string scoreText2 = std::to_string(scorePaddle2);

        //update direction every frame
        direction = moveBall(deltaTime);

        //detect if anyone won
        if (scorePaddle1 >= 10 || scorePaddle2 >= 10) {
            gameOver = true;
            handleGameOver(window, font);
        }
    }

    //function to draw the paddles, ball and the score text
    void draw(sf::RenderWindow& window) {
        window.draw(paddle1.getShape());
        window.draw(paddle2.getShape());
        window.draw(ball.getShape());

        sf::Text Score1(std::to_string(scorePaddle1), font, 100);
        Score1.setFillColor(sf::Color::White);
        Score1.setPosition(500, 30);

        sf::Text Score2(std::to_string(scorePaddle2), font, 100);
        Score2.setFillColor(sf::Color::White);
        Score2.setPosition(650,30);

        window.draw(Score1);
        window.draw(Score2);

        for (int i = 0; i < 19; i++){
            sf::RectangleShape square(sf::Vector2f(BALL_SIDE, BALL_SIDE));
            square.setPosition(SCREEN_WIDTH/2 - 10, BALL_SIDE + BALL_SIDE * 2 * i);
            window.draw(square);
        }
    }

    //getters
    bool isGameOver() const {
        return gameOver;
    }

    int getScorePaddle1() const {
        return scorePaddle1;
    }

    int getScorePaddle2() const {
        return scorePaddle2;
    }

    bool getStartUpChoice() const {
        return startUpChoice;
    }

    void setGameOver(bool value) {
        gameOver = value;
    }

private:
    Paddle paddle1;
    Paddle paddle2;
    Ball ball;
    sf::Vector2f direction;
    int scorePaddle1;
    int scorePaddle2;
    bool computer;
    bool gameOver;
    bool startUpChoice;

    //takes references to both the paddles as input and moves them based on keyboard input
    void movePaddle(Paddle& paddle1, Paddle& paddle2, float deltaTime) {
        const float paddleSpeed = 450.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
            float nextPosition = paddle1.getShape().getPosition().y;
            nextPosition -= paddleSpeed * deltaTime;
            if (nextPosition > 0) //checks if the paddle would still be within the screen if the current position is set to the calculated next position
                paddle1.move(-paddleSpeed * deltaTime);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
            float nextPosition = paddle1.getShape().getPosition().y;
            nextPosition += paddleSpeed * deltaTime;
            if (nextPosition < SCREEN_HEIGHT - PADDLE_HEIGHT)
                paddle1.move(paddleSpeed * deltaTime);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
            float nextPosition = paddle2.getShape().getPosition().y;
            nextPosition -= paddleSpeed * deltaTime;
            if (nextPosition > 0)
                paddle2.move(-paddleSpeed * deltaTime);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
            float nextPosition = paddle2.getShape().getPosition().y;
            nextPosition += paddleSpeed * deltaTime;
            if (nextPosition < SCREEN_HEIGHT - PADDLE_HEIGHT)
                paddle2.move(paddleSpeed * deltaTime);
        }
    }

    //uses SFML's getGLobalBounds and intersect functions to draw a rectangle around both the ball
    //and the paddle and returns true if the rectangles intersect
    bool isCollision(const sf::RectangleShape& ball, const sf::RectangleShape& paddle) {
        return ball.getGlobalBounds().intersects(paddle.getGlobalBounds());
    }

    void computerPlayer(float deltaTime) {
        const float paddleSpeed = 450.0f;
        
        //predict the ball's position and adjust the paddle's position accordingly
        float targetY = ball.getShape().getPosition().y;

        //if ball is in the opponent half of the screen, add/subtract from targetY accordingly 
        if (ball.getShape().getPosition().x > SCREEN_WIDTH / 2) {
            targetY += (ball.getShape().getPosition().y > paddle1.getShape().getPosition().y) ? paddleSpeed * deltaTime : -paddleSpeed * deltaTime;
        }

        targetY = std::max(0.0f, std::min(SCREEN_HEIGHT - PADDLE_HEIGHT, targetY));

        float currentY = paddle1.getShape().getPosition().y;
        float direction = (targetY > currentY) ? 1.0f : -1.0f;
        float nextPosition = currentY + direction * paddleSpeed * deltaTime;

        paddle1.move(nextPosition - currentY);
    }

    sf::Vector2f moveBall(float deltaTime) {
        float speed = 800.0f;

    //handling collision with the paddle by negating the x component of the vector (180 degree turn) and giving y direction a random value
        if (isCollision(ball.getShape(), paddle1.getShape()) || isCollision(ball.getShape(), paddle2.getShape())){
            direction.x = -direction.x;
            direction.y = -0.9 + static_cast<float>(std::rand()) / RAND_MAX * (0.9 + 0.9);

            //set the ball location to (1+end of the paddle, current y position)
            if (isCollision(ball.getShape(), paddle1.getShape())){
                ball.getShape().setPosition(21, ball.getShape().getPosition().y);
            }

            if (isCollision(ball.getShape(), paddle2.getShape())){
                ball.getShape().setPosition(1159, ball.getShape().getPosition().y);
            }
        }
    
        //handling collision with the top and bottom of the screen
        if (ball.getShape().getPosition().y < 1){
            direction.y = -direction.y;
            ball.getShape().setPosition(ball.getShape().getPosition().x, 1);
        } else if (ball.getShape().getPosition().y > SCREEN_HEIGHT - BALL_SIDE){
            direction.y = -direction.y;
            ball.getShape().setPosition(ball.getShape().getPosition().x, SCREEN_HEIGHT - BALL_SIDE);
        }

        //calculate the next position of the ball, deltaTime for making it framerate independent
        sf::Vector2f nextPosition = ball.getShape().getPosition() + (speed * direction * deltaTime);

        //set the current position to the calculated next position if the ball is inside the screen
        if (nextPosition.x > 0 && nextPosition.x < SCREEN_WIDTH && nextPosition.y > 0 && nextPosition.y < SCREEN_HEIGHT){
            ball.move(nextPosition - ball.getShape().getPosition());
        }

        //if the ball is offscreen, reverse its x direction (so it flies towards the player who scored)
        //and reset the paddles and ball to their initial positions
        if (nextPosition.x < 5 || nextPosition.x > SCREEN_WIDTH - 5) {
            if (nextPosition.x < 5) {
                scorePaddle2++;
            } else {
                scorePaddle1++;
            }
            direction.x = - direction.x;
            direction.y = 0;
            ball.getShape().setPosition(SCREEN_WIDTH / 2 - BALL_SIDE / 2, SCREEN_HEIGHT / 2 - BALL_SIDE / 2);
        }

        return direction;
    }
};

int main() {
    PongGame game;

    //creating a window
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Pong");
    sf::Font font;

    //import a .ttf font file
    if (!font.loadFromFile("C:\\Users\\devan\\Downloads\\VT323\\pixelfont.ttf")) {
        return EXIT_FAILURE;
    }

    //clock to keep tracks of time elapsed in between frames
    sf::Clock clock;

    while (window.isOpen()) {   
        //SFML event class, stores events
        sf::Event event;

        while (window.pollEvent(event)) {
            //checking if window needs to be closed
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        //if startup choice has not been made yet we display the start screen
        if (!game.getStartUpChoice()) {
            game.handleInput(window, font);
        }

        //if startup choice has been made and the game is not over yet, we continuously update the screen
        if (game.getStartUpChoice() && !game.isGameOver()) {
            //time between frames, used to make the game framerate independent
            float deltaTime = clock.restart().asSeconds();
            game.update(deltaTime);

            window.clear();
            game.draw(window);
            window.display();
        }
    }

    return 0;
}