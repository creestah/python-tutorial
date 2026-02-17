#include <iostream>
#include <string>

int main() {
    std::string name;
    int age;

    std::cout << "Enter your name: ";
    std::cin >> name; // Reads input until the first space

    std::cout << "Enter your age: ";
    std::cin >> age;

    std::cout << "Hello " << name << ", you are " << age << " years old.";
    
    return 0;
}