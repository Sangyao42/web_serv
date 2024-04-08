#include "memory.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "arena.hpp"

// typedef std::string ArenaString;
// typedef std::vector<int> ArenaIntVector;
namespace ws
{
  typedef std::basic_string<char, std::char_traits<char>, ArenaAllocator<char, &arena::temporary> > string;

  template <class T, Arena* arena = &arena::temporary>
  struct vector : public std::vector<T, ArenaAllocator<T, arena> > {};
} // namespace ws


int main() {
    ws::string a;
    a = "Hello, world!";
    a += " This is a test.";
    std::cout << a << std::endl;

    {
      ws::vector<int> b;

      b.push_back(1);
      b.push_back(2);
      b.push_back(3);

      for (ws::vector<int>::const_iterator it = b.begin(); it != b.end(); ++it) {
          std::cout << *it << std::endl;
      }

    }
    {
      ws::vector<int> c;

      c.push_back(1);
      c.push_back(2);
      c.push_back(3);

      for (ws::vector<int>::const_iterator it = c.begin(); it != c.end(); ++it) {
          std::cout << *it << std::endl;
      }

    }

    return EXIT_SUCCESS;
}
