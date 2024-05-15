/*
    Coded developed by VorahPong, Mean
    4/14/2024
*/

#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <deque>

//Define custom colors
Color green = { 100, 200, 100, 200 };

//Creating cells related to the resolution
//30x25 = 750
int cellSize = 30;
int cellCount = 25;

//Score
int score = 0;

//Game status
bool paused = false;
bool startGame = false;

//button
bool startBtn = false;
bool exitBtn = false;

//mouse position
Vector2 mousePoint = {0.0f, 0.0f};

//Move per time for snake
double lastUpdateTime = 0;

bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

//Snake
class Snake{
public:
    std::deque<Vector2> body = { Vector2{6,9},Vector2{5,9},Vector2{4,9} };
    //Facing which direction
    Vector2 direction = { 1,0 };

    //Snake should get longer after eating food and plus 1 score
    void EatFood() {
        body.push_back({0,0});
        score++;
    }

    //Draw snake
    void Draw() {
        int i, x, y;
        //Draw head first
        x = body[0].x;
        y = body[0].y;
        DrawRectangle(x * cellSize, y * cellSize, cellSize, cellSize, RED);
        //Rest of the body
        for (i = 1; i < body.size(); i++) {
            x = body[i].x;
            y = body[i].y;
            DrawRectangle(x * cellSize, y * cellSize, cellSize-1, cellSize-1, DARKBLUE);
        }
    }

    //update movement
    void Update() {
        body.pop_back();
        body.push_front(Vector2Add(body[0], direction));
    }

private:
};

//Food the snake eat
class Food {
public:
    Vector2 position{};  //coordination of the food by cell

    Food() { // default constructor
        position = GenerateRandomPos();
    }

    void Draw() {   //Food shape
        DrawRectangle(position.x * cellSize, position.y * cellSize, cellSize, cellSize, green); //draw a circle to indicate food
    }

    Vector2 GenerateRandomPos() { //generate random food spawn point
        float x = GetRandomValue(1, cellCount - 2);
        float y = GetRandomValue(1, cellCount - 2);
        return Vector2{ x, y };
    }


private:

};

//Game
class Game {
public:
    Snake snake = Snake();
    Food food = Food();

    // Draw food and snake
    void Draw() {
        food.Draw();
        snake.Draw();
    }

    void Update() {
        snake.Update();
    }

    // Check if the snake is touching food
    void CheckCollisionWithFood() {
        if(Vector2Equals(snake.body[0], food.position)) {
            std::cout << "Food is eaten\n";
            food.position = food.GenerateRandomPos();
            snake.EatFood();
        }
    }

    // Check if the snake is touching itself WOAH PAUSED.
    bool CheckCollisionWithItSelf() {
        for(int i = 1; i < snake.body.size(); i++) {
            if(Vector2Equals(snake.body[0], snake.body[i])) {
                std::cout << "Snake has collided with it self. You lose!\n";
                return true;
            }
        }
        return false;
    }

    // Check is snake is collide with edge of the screen
    bool CheckCollisionWithEdgeScreen() {
        if(snake.body[0].x == 0 || snake.body[0].x == cellCount-1 || snake.body[0].y == 0 || snake.body[0].y == cellCount-1) {
            std::cout << "Snake has collide with the edge of the screen\n";
            return true;
        }
        return false;
    }
};

int main() {


    std::cout << "Starting the game..." << std::endl;
    InitWindow(cellSize * cellCount, cellSize * cellCount, "SNAKE");  // Create Window with resolution 750x750 titled "SNAKE"
    SetTargetFPS(60); // set the game frame per second, if not defined then the game would run too fast

    Game game = Game();

    //Game loop
    while (WindowShouldClose() == false) {  // Condition is if user press escape or click the x button then the while loop will break
        mousePoint = GetMousePosition();
        BeginDrawing();

        if(startGame == false) {

            ClearBackground(MAROON);
            DrawRectangle(250, 400, 240, 90, BLACK);
            Rectangle startButton = { 250, 400, 240, 90};
            DrawText("Play", 280, 400, 80, WHITE);

            DrawRectangle(250, 600, 240, 90, BLACK);
            Rectangle exitButton = { 250, 600, 240, 90};
            DrawText("Exit", 280, 600, 80, WHITE);

            if(CheckCollisionPointRec(mousePoint, startButton)) {
                if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    startGame = true;
                }
            }
            if(CheckCollisionPointRec(mousePoint, exitButton)) {
                if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    break;
            }


        }
        else if(paused == true) { // if game is paused
            DrawText("YOU LOST!", cellSize+100, cellSize+100, 90, RED);
            DrawText("Press 'Y' to try again", cellSize+300, cellSize+300, 30, RED);
            if(IsKeyPressed(KEY_Y)) {
                paused = false;
                score = 0;
                while(game.snake.body.size() > 3) {
                    game.snake.body.pop_back();
                }
                game.snake.body = { Vector2{6,9},Vector2{5,9},Vector2{4,9} };
                game.snake.direction = { 1,0 };
            }
        }
        else if(startGame == true){

            //draw the black outline around the screen
            for(int index = 0; index < cellCount; index++) {
                DrawRectangle(0*cellSize, index*cellSize, cellSize, cellSize, BLACK); // left
                DrawRectangle((cellCount-1)*cellSize, index*cellSize, cellSize, cellSize, BLACK); // right
                DrawRectangle(index*cellSize, 0*cellSize, cellSize, cellSize, BLACK); // top
                DrawRectangle(index*cellSize, (cellCount-1)*cellSize, cellSize, cellSize, BLACK); // down
            }

            DrawText(TextFormat("Score: %03i", score), 0, 0, 30, GREEN);

            ClearBackground(DARKBROWN); //set background color to green

            // draw the food, and the snake
            game.Draw();

            //Snake move every 0.2 sec
            if (eventTriggered(0.2)) {
                game.Update();
            }

            //Movement pressed
            //using arrow keys
            if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && game.snake.direction.y != 1) { //move up
                game.snake.direction = { 0, -1 };
            }
            if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && game.snake.direction.y != -1) { //move down
                game.snake.direction = { 0, 1 };
            }
            if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && game.snake.direction.x != -1) {  //move right
                game.snake.direction = { 1, 0 };
            }
            if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && game.snake.direction.x != 1) {   //move left
                game.snake.direction = { -1, 0 };
            }
            game.CheckCollisionWithFood();
            if(game.CheckCollisionWithItSelf() || game.CheckCollisionWithEdgeScreen()) {
                paused = true;
            }
        }
        EndDrawing(); // end drawing
    }

    CloseWindow();  //Closed the window
    
    return 0;
}