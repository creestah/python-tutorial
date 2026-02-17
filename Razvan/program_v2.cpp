#include <iostream>
#include <string>
using namespace std;

int main() {
    string name;
    int age;

    cout << "Enter your name: ";
    cin >> name; // Reads input until the first space

    cout << "Enter your age: ";
    cin >> age;

    cout << "Hello " << name << ", you are " << age << " years old.";
    
    return 0;
}