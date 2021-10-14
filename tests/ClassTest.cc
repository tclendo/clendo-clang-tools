#include <iostream>

class Parent {
public:
    Parent() {
        std::cout << "Constructor for Parent";
    }

    ~Parent() {
        std::cout << "Destructor for Parent";
    }
};

class Child : public Parent {
public:
    Child(Parent parent) : parent(parent) {
        std::cout << "Constructor for Child";
    }

    ~Child() {
        std::cout << "Destructor for Child";
    }

private:
    Parent parent;
};

int main(int argv, char** argc) {
    Parent parent;
    Child child = Child(parent);
    return 0;
}
