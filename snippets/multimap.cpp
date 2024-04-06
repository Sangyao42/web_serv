#include <iostream>
#include <map>
#include <utility>

typedef std::multimap<int, char>    MM;
typedef std::pair<MM::iterator, MM::iterator>   MM_range;

void    print_status(MM_range range, MM& dict)
{
  std::cout << "range   : " << (range.first == dict.end()) << (range.second == dict.end()) << std::endl;
  std::cout << "is_equal: " << (range.first == range.second) << std::endl;
}

void    print_result(MM_range range)
{
  for (MM::iterator i = range.first; i != range.second; ++i)
    std::cout << i->first << ": " << i->second << '\n';
}

void    print(MM_range range, MM& dict, int n)
{
  std::cout << "searching on " << n << std::endl;
  print_status(range, dict);
  print_result(range);
  std::cout << std::endl;
}
 
int main()
{
  std::multimap<int, char> dict;
  dict.insert(std::make_pair(1, 'A'));
  dict.insert(std::make_pair(2, 'B'));
  dict.insert(std::make_pair(2, 'C'));
  dict.insert(std::make_pair(2, 'D'));
  dict.insert(std::make_pair(4, 'E'));
  dict.insert(std::make_pair(3, 'F'));
  
  for (int i = 0; i < 6; i++)
  {
    print(dict.equal_range(i), dict, i);
  }

  {
    // Normal case
    MM_range saved = dict.equal_range(2);
    dict.insert(std::make_pair(2, '0'));
    dict.insert(std::make_pair(2, 'G'));
    print(saved, dict, 2);
  }

  {
    // case when the upper range is end of the map
    MM_range saved = dict.equal_range(4);
    dict.insert(std::make_pair(4, '0'));
    dict.insert(std::make_pair(4, 'H'));
    print(saved, dict, 4);
  }

  {
    // invalid range before insertion
    MM_range saved = dict.equal_range(5);
    dict.insert(std::make_pair(5, 'I'));
    print(saved, dict, 5);
  }
  
  {
    // upper range is end of the map, then insert a new element
    // to the end. The range therefore is invalid because it would
    // also include the new element {6, 'J'}
    MM_range saved = dict.equal_range(5);
    dict.insert(std::make_pair(6, 'J'));
    print(saved, dict, 5);
  }
  
  {
    // When both end of the range is clear, the range is valid
    MM_range saved = dict.equal_range(4);
    dict.insert(std::make_pair(5, 'K'));
    print(saved, dict, 4);
  }
  
  {
    MM_range saved = dict.equal_range(6);
    dict.insert(std::make_pair(8, 'L'));
    print(saved, dict, 6);
  }
}
