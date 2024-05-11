#pragma once

#include <string>
#include <vector>

#include "Arena.hpp"

namespace permanent
{
  typedef std::basic_string<char, std::char_traits<char>, ArenaAllocator<char, &arena::permanent> > string;

  template <class T>
  struct vector : public std::vector<T, ArenaAllocator<T, &arena::permanent> > {};

} // namespace permanent

namespace temporary
{
  typedef std::basic_string<char, std::char_traits<char>, ArenaAllocator<char, &arena::temporary> > string;

  template <class T>
  struct vector : public std::vector<T, ArenaAllocator<T, &arena::temporary> > {};

} // namespace temporary

