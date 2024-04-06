#include <cstdlib>
#include <iostream>

class A
{
  public:
    A() {};
    A(const A&) {};

    A& operator=(const A&) { return *this; };
    virtual ~A() {};
};

class B : public A
{
  public:
    B() : A(), _foo(1) {};
    B(const B& b) : A(), _foo(b._foo) {};
    B& operator=(const B& b) { _foo = b._foo; return *this; };
    virtual ~B() {};

    void  foo() {
      std::cout << _foo << std::endl;
    }
  
  private:
    int _foo;
};

int main()
{
  A* a = new B();
  static_cast<B*>(a)->foo();
  return EXIT_SUCCESS;
}
