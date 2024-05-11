#pragma once

#include <cassert>
#include <cstddef>

#include <stdint.h>
#include <limits>

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif

typedef unsigned char*  ArenaSnapshot;

class Arena
{
public:
  Arena(size_t size);
  ~Arena();

  template <typename T>
  T *allocate(size_t count = 1);
  void clear();
  ArenaSnapshot snapshot();
  void rollback(ArenaSnapshot snapshot);

private:
  unsigned char *memory_;
  unsigned char *fill_;
  size_t size_;

  Arena();
  Arena(const Arena &other);
  Arena &operator=(const Arena &other);

  uintptr_t align(uintptr_t ptr, size_t alignment);
};

template <typename T>
T* Arena::allocate(size_t count) {
    const size_t size = sizeof(T) * count;
    assert((fill_ + size < (memory_ + size_)) && "Arena out of memory");
    T* result = ::new (fill_) T[count];
    fill_ += size;
    return result;
}

template <class T, Arena* arena>
class ArenaAllocator
{
public:
    typedef T                 value_type;
    typedef value_type&       reference;
    typedef value_type const& const_reference;
    typedef value_type*       pointer;
    typedef value_type const* const_pointer;
    typedef std::size_t       size_type;
    typedef std::ptrdiff_t    difference_type;

    template <class U>
    struct rebind
    {
        typedef ArenaAllocator<U, arena> other;
    };

    ArenaAllocator() throw() {}  // not required, unless used
    template <class U> ArenaAllocator(ArenaAllocator<U, arena> const&) throw() {}

    pointer
    allocate(size_type n, typename ArenaAllocator<void, arena>::const_pointer = 0)
    {
        return static_cast<pointer>(arena->allocate<T>(n));
    }

    void
    deallocate(pointer, size_type) {}

    void
    construct(pointer p, value_type const& val)
    {
        ::new(p) value_type(val);
    }

    void
    destroy(pointer) {}

    size_type
    max_size() const throw()
    {
        return std::numeric_limits<size_type>::max() / sizeof(value_type);
    }

    pointer
    address(reference x) const
    {
        return &x;
    }

    const_pointer
    address(const_reference x) const
    {
        return &x;
    }
};

template <class T, class U, Arena* arena>
bool
operator==(ArenaAllocator<T, arena> const&, ArenaAllocator<U, arena> const&)
{
    return true;
}

template <class T, class U, Arena* arena>
bool
operator!=(ArenaAllocator<T, arena> const& x, ArenaAllocator<U, arena> const& y)
{
    return !(x == y);
}
