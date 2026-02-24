#include <string>
#include <iostream>
#include <chrono>
#include <cstdlib>
using namespace std;

struct Vec2 { int x, y; };

void drawBoard( int w, int h, Vec2 head) {
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

    drawBoard( boardWidth, boardHeight, head);

    return 0;
}