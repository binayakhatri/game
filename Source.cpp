#include <GLFW/glfw3.h>
#include <iostream>
#include <deque>       // Provides a double-ended queue container
#include <cstdlib>     // Provides general utilities: srand(), rand()
#include <ctime>       // Provides functions to work with time: time(), time_t
#include <chrono>      // Provides functions and classes to measure time: duration, system_clock
#include <thread>      // Provides classes and functions to work with threads: this_thread, sleep_for()
using namespace std;

const int WIDTH = 800;
const int HEIGHT = 600;
const int BLOCK_SIZE = 20;
const int INITIAL_SNAKE_LENGTH = 3;
const double GAME_SPEED = 0.1; // Controls the game speed


struct Point
{
    int x;
    int y;
};

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

deque<Point> snake;// Double-ended queue to store snake segments
Direction direction = RIGHT;// Initial direction of the snake
Point food;// Position of the food

// Function to generate food at a random position
void generateFood()
{
    food.x = rand() % (WIDTH / BLOCK_SIZE);
    food.y = rand() % (HEIGHT / BLOCK_SIZE);
}
// Callback function for handling keyboard input
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_UP:
            if (direction != DOWN)
                direction = UP;
            break;
        case GLFW_KEY_DOWN:
            if (direction != UP)
                direction = DOWN;
            break;
        case GLFW_KEY_LEFT:
            if (direction != RIGHT)
                direction = LEFT;
            break;
        case GLFW_KEY_RIGHT:
            if (direction != LEFT)
                direction = RIGHT;
            break;
        }
    }
}
// Function to update the snake's position and check for collisions
void updateSnake()
{
    Point newHead = { snake.front().x, snake.front().y };
    switch (direction)
    {
    case UP:
        newHead.y--;
        break;
    case DOWN:
        newHead.y++;
        break;
    case LEFT:
        newHead.x--;
        break;
    case RIGHT:
        newHead.x++;
        break;
    }

    // Check if the snake eats the food
    if (newHead.x == food.x && newHead.y == food.y)
    {
        generateFood();
    }
    else
    {
        // Remove the tail segment if no food is eaten
        snake.pop_back();
    }

    // Check for collisions with walls
    if (newHead.x < 0 || newHead.x >= WIDTH / BLOCK_SIZE ||
        newHead.y < 0 || newHead.y >= HEIGHT / BLOCK_SIZE)
    {
        // Game over, reset the snake
        snake.clear();
        Point startPos = { WIDTH / BLOCK_SIZE / 2, HEIGHT / BLOCK_SIZE / 2 };
        for (int i = 0; i < INITIAL_SNAKE_LENGTH; ++i) {
            snake.push_back({ startPos.x - i, startPos.y });
        }
        return;
    }

    // Check for collisions with itself
    for (const auto& segment : snake)
    {
        if (segment.x == newHead.x && segment.y == newHead.y)
        {
            // Game over, reset the snake
            snake.clear();
            Point startPos = { WIDTH / BLOCK_SIZE / 2, HEIGHT / BLOCK_SIZE / 2 };
            for (int i = 0; i < INITIAL_SNAKE_LENGTH; ++i) {
                snake.push_back({ startPos.x - i, startPos.y });
            }
            return;
        }
    }

    // Add the new head segment
    snake.push_front(newHead);
}
// Function to draw a block at the specified position
void drawBlock(int x, int y) {
    glBegin(GL_QUADS);
    glVertex2i(x * BLOCK_SIZE, y * BLOCK_SIZE);
    glVertex2i((x + 1) * BLOCK_SIZE, y * BLOCK_SIZE);
    glVertex2i((x + 1) * BLOCK_SIZE, (y + 1) * BLOCK_SIZE);
    glVertex2i(x * BLOCK_SIZE, (y + 1) * BLOCK_SIZE);
    glEnd();
}
// Function to draw the snake
void drawSnake() {
    glColor3f(0.0f, 1.0f, 0.0f); // Snake color
    for (const auto& segment : snake) {
        drawBlock(segment.x, segment.y);
    }
}
// Function to draw the food at its current position
void drawFood() {
    glColor3f(1.0f, 0.0f, 0.0f); // Food color
    drawBlock(food.x, food.y);// Draw the food block
}
// Initialize GLFW
int main() {
    if (!glfwInit())
        return -1;
    // Create a window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Snake", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);// Set keyboard input callback

    glOrtho(0, WIDTH, HEIGHT, 0, 0, 1);// Set up orthographic projection


    srand(static_cast<unsigned int>(time(NULL))); // Seed random number generator
    generateFood();// Generate initial food position

    // Initialize snake's initial posit
    Point startPos = { WIDTH / BLOCK_SIZE / 2, HEIGHT / BLOCK_SIZE / 2 };
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; ++i) {
        snake.push_back({ startPos.x - i, startPos.y });
    }

    double lastFrame = glfwGetTime();// Get the time of the last frame
    while (!glfwWindowShouldClose(window)) {// Main game loop
        double currentFrame = glfwGetTime();// Get the current time
        double deltaTime = currentFrame - lastFrame;// Calculate time difference

        if (deltaTime >= GAME_SPEED) {// Update game logic if enough time has passed
            glClear(GL_COLOR_BUFFER_BIT);
            updateSnake(); 
            drawSnake();
            drawFood();
            glfwSwapBuffers(window);// Swap the front and back buffers
            glfwPollEvents();
            lastFrame = currentFrame;// Update the last frame time
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));// Pause briefly to control game speed
        }
    }

    glfwTerminate();// Terminate GLFW
    return 0;
}
