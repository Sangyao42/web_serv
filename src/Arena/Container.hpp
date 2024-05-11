#pragma once

#include <string>
#include <vector>

#include "Arena.hpp"

namespace permanent
{
  extern Arena arena;
  typedef std::basic_string<char, std::char_traits<char>, ArenaAllocator<char, &arena> > string;

  template <class T>
  struct vector : public std::vector<T, ArenaAllocator<T, &arena> > {};

} // namespace permanent

namespace temporary
{
  extern Arena arena;
  typedef std::basic_string<char, std::char_traits<char>, ArenaAllocator<char, &arena> > string;

  template <class T>
  struct vector : public std::vector<T, ArenaAllocator<T, &arena> > {};

} // namespace temporary

// These template specification is required so that the allocator works when type T is void.
// I don't understand fully, but that is one of the requirement from C++

template <>
class ArenaAllocator<void, &permanent::arena>
{
public:
    typedef void              value_type;
    typedef value_type*       pointer;
    typedef value_type const* const_pointer;
    typedef std::size_t       size_type;
    typedef std::ptrdiff_t    difference_type;

    template <class U>
    struct rebind
    {
        typedef ArenaAllocator<U, &permanent::arena> other;
    };
};

template <>
class ArenaAllocator<void, &temporary::arena>
{
public:
    typedef void              value_type;
    typedef value_type*       pointer;
    typedef value_type const* const_pointer;
    typedef std::size_t       size_type;
    typedef std::ptrdiff_t    difference_type;

    template <class U>
    struct rebind
    {
        typedef ArenaAllocator<U, &temporary::arena> other;
    };
};

