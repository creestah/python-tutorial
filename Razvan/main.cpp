// Snake game (beginner-friendly version)
// - Uses simple arrays (no advanced data structures)
// - Uses real-time input (WASD, plus arrow keys on Windows)
// - Wraps around the edges instead of hitting walls

#include <chrono>   // for timing
#include <cstdlib>  // for rand, system
#include <ctime>    // for time (random seed)
#include <iostream> // for cout, cin
#include <limits>   // for numeric_limits
#include <thread>   // for sleep_for

// Platform-specific includes for real-time keyboard input
#ifdef _WIN32
#include <conio.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

// A simple pair of coordinates
struct Position {
    int x;
    int y;
};

// Possible movement directions
enum Direction { Up, Down, Left, Right };

// -----------------------------
// Real-time input (platform-specific)
// -----------------------------
#ifdef _WIN32
// On Windows, we can check if a key is pressed with _kbhit() and read it with _getch().
int readInputIfAny() {
    if (_kbhit()) {
        int ch = _getch();

        // Arrow keys on Windows are a two-step code.
        if (ch == 0 || ch == 224) {
            int ext = _getch();
            switch (ext) {
                case 72: return 'w'; // Up
                case 80: return 's'; // Down
                case 75: return 'a'; // Left
                case 77: return 'd'; // Right
                default: return -1;
            }
        }

        // If it's a normal key (like 'w'), just return it.
        return ch;
    }

    // No key pressed.
    return -1;
}
#else
// On macOS/Linux, we temporarily put the terminal in raw, non-blocking mode
// so we can read key presses without waiting for Enter.
struct TerminalRawMode {
    termios originalSettings{};
    int originalFlags = 0;

    TerminalRawMode() {
        // Save current terminal settings.
        tcgetattr(STDIN_FILENO, &originalSettings);

        // Make a copy and disable canonical mode and echo.
        termios raw = originalSettings;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 0;  // return immediately
        raw.c_cc[VTIME] = 0; // no timeout
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);

        // Make STDIN non-blocking.
        originalFlags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, originalFlags | O_NONBLOCK);
    }

    ~TerminalRawMode() {
        // Restore terminal settings when we exit.
        tcsetattr(STDIN_FILENO, TCSANOW, &originalSettings);
        fcntl(STDIN_FILENO, F_SETFL, originalFlags);
    }
};

// Try to read one character. If there is none, return -1.
int readInputIfAny() {
    unsigned char ch = 0;
    ssize_t n = read(STDIN_FILENO, &ch, 1);
    if (n == 1) return ch;
    return -1;
}
#endif

// -----------------------------
// Game helpers
// -----------------------------

// Clear the screen (different command for Windows vs macOS/Linux).
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Check if two directions are exact opposites (Up vs Down, Left vs Right).
bool areOpposite(Direction a, Direction b) {
    return (a == Up && b == Down) ||
           (a == Down && b == Up) ||
           (a == Left && b == Right) ||
           (a == Right && b == Left);
}

// Wrap around the edges of the board.
// Example: if you go left from x = 0, you appear at x = width - 1.
Position wrapAround(Position p, int width, int height) {
    if (p.x < 0) p.x = width - 1;
    else if (p.x >= width) p.x = 0;

    if (p.y < 0) p.y = height - 1;
    else if (p.y >= height) p.y = 0;

    return p;
}

// Check if a position is already occupied by the snake.
bool isOnSnake(int x, int y, int snakeX[], int snakeY[], int snakeLength) {
    for (int i = 0; i < snakeLength; i++) {
        if (snakeX[i] == x && snakeY[i] == y) return true;
    }
    return false;
}

// Generate food in a random empty cell.
Position spawnFood(int width, int height, int snakeX[], int snakeY[], int snakeLength) {
    Position food;

    do {
        food.x = rand() % width;
        food.y = rand() % height;
    } while (isOnSnake(food.x, food.y, snakeX, snakeY, snakeLength));

    return food;
}

// Draw the entire game board.
void drawBoard(int width, int height, int snakeX[], int snakeY[], int snakeLength, Position food, int score) {
    clearScreen();

    // Keep a simple fixed-size board for beginner-friendly code.
    // Make sure width <= 50 and height <= 25.
    char board[25][50];

    // Fill the board with empty spaces.
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            board[y][x] = ' ';
        }
    }

    // Place the food.
    board[food.y][food.x] = 'O';

    // Place the snake.
    for (int i = 0; i < snakeLength; i++) {
        int x = snakeX[i];
        int y = snakeY[i];
        board[y][x] = (i == 0) ? 'Q' : 'o';
    }

    // Top border
    for (int x = 0; x < width + 2; x++) cout << '#';
    cout << "\n";

    // Middle rows
    for (int y = 0; y < height; y++) {
        cout << '#';
        for (int x = 0; x < width; x++) cout << board[y][x];
        cout << "#\n";
    }

    // Bottom border
    for (int x = 0; x < width + 2; x++) cout << '#';
    cout << "\n";

    // Score display
    cout << "Score: " << score << "\n";
}

// Read input and update direction.
void handleInput(Direction &currentDirection, Direction &nextDirection) {
    int inputChar = readInputIfAny();
    if (inputChar == -1) return; // no key pressed

    if (inputChar == 'w' || inputChar == 'W') nextDirection = Up;
    else if (inputChar == 's' || inputChar == 'S') nextDirection = Down;
    else if (inputChar == 'a' || inputChar == 'A') nextDirection = Left;
    else if (inputChar == 'd' || inputChar == 'D') nextDirection = Right;

    // Prevent instant 180-degree turns.
    if (!areOpposite(currentDirection, nextDirection)) {
        currentDirection = nextDirection;
    }
}

// Move the snake and handle collisions and food.
// Returns false if the snake hits itself (game over).
bool updateSnake(int width, int height,
                 int snakeX[], int snakeY[], int &snakeLength, int maxSnakeLength,
                 Direction currentDirection, Position &food, int &score) {
    // Figure out where the new head will be.
    Position newHead;
    newHead.x = snakeX[0];
    newHead.y = snakeY[0];

    if (currentDirection == Up) newHead.y--;
    else if (currentDirection == Down) newHead.y++;
    else if (currentDirection == Left) newHead.x--;
    else if (currentDirection == Right) newHead.x++;

    // Wrap around the edges.
    newHead = wrapAround(newHead, width, height);

    // If the new head is on the snake body, the game ends.
    if (isOnSnake(newHead.x, newHead.y, snakeX, snakeY, snakeLength)) {
        return false;
    }

    // Check if we are eating food.
    bool ateFood = (newHead.x == food.x && newHead.y == food.y);

    // If we eat, the snake grows by 1 (up to max length).
    int newLength = snakeLength;
    if (ateFood && snakeLength < maxSnakeLength) {
        newLength = snakeLength + 1;
    }

    // Move the body: start from the tail and shift each segment forward.
    for (int i = newLength - 1; i > 0; i--) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    // Put the new head at the front.
    snakeX[0] = newHead.x;
    snakeY[0] = newHead.y;
    snakeLength = newLength;

    // If we ate food, increase score and spawn new food.
    if (ateFood) {
        score += 10;
        food = spawnFood(width, height, snakeX, snakeY, snakeLength);
    }

    return true; // still alive
}

int main() {
    cout << "Welcome to the Snake game!\n";
    cout << "Controls: W A S D (and arrow keys on Windows).\n\n";

    // Board size
    const int boardWidth = 30;
    const int boardHeight = 16;

    // Game speed (lower = faster)
    const int millisecondsPerTick = 160;

    // The snake is stored in two simple arrays.
    // Index 0 is the head.
    const int maxSnakeLength = boardWidth * boardHeight;
    int snakeX[maxSnakeLength];
    int snakeY[maxSnakeLength];
    int snakeLength = 3;

    // Starting position of the snake (3 segments long).
    snakeX[0] = boardWidth / 2;
    snakeY[0] = boardHeight / 2;
    snakeX[1] = snakeX[0] - 1;
    snakeY[1] = snakeY[0];
    snakeX[2] = snakeX[0] - 2;
    snakeY[2] = snakeY[0];

    // Direction setup
    Direction currentDirection = Right;
    Direction nextDirection = currentDirection;

    // Random seed for food placement
    srand((unsigned int)time(nullptr));

    // Place the first food
    Position food = spawnFood(boardWidth, boardHeight, snakeX, snakeY, snakeLength);

    int score = 0;
    bool gameOver = false;
    auto lastUpdateTime = chrono::steady_clock::now();

    // Draw the first frame
    drawBoard(boardWidth, boardHeight, snakeX, snakeY, snakeLength, food, score);

// Set up real-time input on macOS/Linux only.
#ifdef _WIN32
    while (!gameOver) {
#else
    TerminalRawMode terminalMode;
    while (!gameOver) {
#endif
        // 1) Read input
        handleInput(currentDirection, nextDirection);

        // 2) Update the game on a fixed timer
        auto now = chrono::steady_clock::now();
        if (chrono::duration_cast<chrono::milliseconds>(now - lastUpdateTime).count() >= millisecondsPerTick) {
            lastUpdateTime = now;

            bool stillAlive = updateSnake(boardWidth, boardHeight,
                                          snakeX, snakeY, snakeLength, maxSnakeLength,
                                          currentDirection, food, score);
            if (!stillAlive) {
                gameOver = true;
                break;
            }

            drawBoard(boardWidth, boardHeight, snakeX, snakeY, snakeLength, food, score);
        }

        // Small sleep to avoid maxing out the CPU.
        this_thread::sleep_for(chrono::milliseconds(5));
    }

    cout << "\nGame Over! Final score: " << score << "\n";
    cout << "Press Enter to exit...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();

    return 0;
}
