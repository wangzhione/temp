#include <iostream>

class A {
    int value;

public:
    A(int n) { this->value = n; }
    /*
     类 "A" 的复制构造函数不能带有 "A" 类型的参数

     20_construction.cc:8:14: error: invalid constructor; you probably meant ‘A (const A&)’
     A(A other) { this->value = other.value; }
     */
    A(A other) { this->value = other.value; }

    void print() { std::cout << value << std::endl; }
};

//
// build:
// g++ -g -O3 -Wall -Wextra -Werror -o 20_construction 20_construction.cc
int main(void) {

    exit(EXIT_SUCCESS);
}