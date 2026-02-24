#include <string>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <thread>
using namespace std;

struct Vec2 { int x, y; };

void clearScreen() {
    system("cls");
}

void drawBoard( int w, int h, Vec2 head) {
    clearScreen();

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
}

int main() {
    cout << "Welcome to the Snake game!\n\n";

    int boardWidth = 30;
    int boardHeight = 12;

    Vec2 head;
    head.x = boardWidth / 2;
    head.y = boardHeight / 2;

    int numberOfMs = 200;
    
    while (true) {
        head.x = head.x + 1;

        if (head.x >= boardWidth) {
            head.x = 0;
        }
    

        drawBoard( boardWidth, boardHeight, head);

        this_thread::sleep_for(chrono::milliseconds(numberOfMs));
    }

    return 0;
}