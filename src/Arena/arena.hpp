#pragma once

#include "memory.hpp"

namespace arena
{
  extern Arena permanent;
  extern Arena temporary;

} // namespace arena

template <>
class ArenaAllocator<void, &arena::permanent>
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
        typedef ArenaAllocator<U, &arena::permanent> other;
    };
};

template <>
class ArenaAllocator<void, &arena::temporary>
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
        typedef ArenaAllocator<U, &arena::temporary> other;
    };
};
