#include <string>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <thread>
#include <conio.h>
using namespace std;

struct Vec2 { int x, y; };

enum class Direction{
    Up,
    Down,
    Left,
    Right
};

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void drawBoard( int w, int h, Vec2 head) {
    clearScreen();

    cout << "Welcome to the Snake game!\n\n";

    // Top border
    for (int x = 0; x < w + 2; x++) {
        cout << "#";
    } 

    cout << "\n";

    // Middle rows
    for (int y = 0; y < h ; y++) {
        cout << "#";

        for (int x = 0; x < w; x++) {
            if (x == head.x && y == head.y) {
                cout << "Q";
            }
            else {
                cout << " ";
            }
        }

        cout << "#\n";
    }

    // Bottom border
    for (int x = 0; x < w + 2; x++) {
        cout << "#";
    }

    cout << "\n";

    cout << "\nWASD to move, Q to quit\n";
}

Vec2 NextPosition (Direction dir, Vec2 head) {
    if (dir == Direction::Up) {
        head.y = head.y - 1;
    }
    if (dir == Direction::Down) {
        head.y = head.y + 1;
    }
    if (dir == Direction::Left) {
        head.x = head.x - 1;
    }
    if (dir == Direction::Right) {
        head.x = head.x + 1;
    }

    return head;
}

bool IsOpposite (Direction dir1, Direction dir2) {
    if (dir1 == Direction::Up && dir2 == Direction::Down) {
        return true;
    }
    if (dir1 == Direction::Down && dir2 == Direction::Up) {
        return true;
    }
    if (dir1 == Direction::Left && dir2 == Direction::Right) {
        return true;
    }
    if (dir1 == Direction::Right && dir2 == Direction::Left) {
        return true;
    }
    return false;
}

Direction KeyToDirection (Direction current, char c) {
    Direction wanted = current;

    if (c == 'w' || c == 'W') {
        wanted = Direction::Up;
    }
    if (c == 's' || c == 'S') {
        wanted = Direction::Down;
    }
    if (c == 'a' || c == 'A') {
        wanted = Direction::Left;
    }
    if (c == 'd' || c == 'D') {
        wanted = Direction::Right;
    }

    if (IsOpposite(wanted, current)) {
        return current;
    }

    return wanted;
}

Vec2 WrapAtWall (Vec2 head, int boardWidth, int boardHeight) {
    if (head.x >= boardWidth) {
        head.x = 0;
    }
    if (head.y >= boardHeight) {
        head.y = 0;
    }
    if (head.x < 0) {
        head.x = boardWidth - 1;
    }
    if (head.y < 0) {
        head.y = boardHeight - 1;
    }
    return head;
}

int main() {
    cout << "Welcome to the Snake game!\n\n";

    int boardWidth = 30;
    int boardHeight = 12;

    Vec2 head;
    head.x = boardWidth / 2;
    head.y = boardHeight / 2;

    int numberOfMs = 200;

    Direction dir = Direction::Up;
    
    while (true) {
        if (_kbhit()) {
            char c = (char) _getch();
            
            if (c == 'q' || c == 'Q') {
                break;
            }

            dir = KeyToDirection(dir, c);
        }

        head = NextPosition(dir, head);

        head = WrapAtWall(head, boardWidth, boardHeight);
    
        drawBoard( boardWidth, boardHeight, head);

        this_thread::sleep_for(chrono::milliseconds(numberOfMs));
    }

    return 0;
}
