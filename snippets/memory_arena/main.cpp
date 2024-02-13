#include "memory.hpp"

#include <cstdlib>
#include <iostream>

struct test {
    int a;
    int b;
    int c;

    test() {
        a = 1;
        b = 2;
        c = 3;
    }

    void print() const {
        std::cout << a << " " << b << " " << c << std::endl;
    }
};

int main() {
    Arena arena(1024);
    test* t = arena.allocate<test>(3);

    t->print();
    t[1].print();
    t[2].print();

    arena.print();

    return EXIT_SUCCESS;
}
