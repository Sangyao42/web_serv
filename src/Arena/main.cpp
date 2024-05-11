#include "Container.hpp"

#include <cstdlib>
#include <iostream>

int sample_main() {
    temporary::string a;
    a = "Hello, world!";
    a += " This is a test.";
    std::cout << a << std::endl;

    {
      temporary::vector<int> b;

      b.push_back(1);
      b.push_back(2);
      b.push_back(3);

      for (temporary::vector<int>::const_iterator it = b.begin(); it != b.end(); ++it) {
          std::cout << *it << std::endl;
      }

    }
    {
      temporary::vector<int> c;

      c.push_back(1);
      c.push_back(2);
      c.push_back(3);

      for (temporary::vector<int>::const_iterator it = c.begin(); it != c.end(); ++it) {
          std::cout << *it << std::endl;
      }

    }

    return EXIT_SUCCESS;
}
