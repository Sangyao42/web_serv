#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif

class Arena;

class Snapshot
{
public:
  Snapshot();
  Snapshot(Arena *arena, size_t offset);
  Snapshot(const Snapshot &other);
  ~Snapshot();
  Snapshot &operator=(const Snapshot &other);

  void  rollback();
  size_t offset() const;

private:
  Arena *arena_;
  size_t offset_;
};

class Arena
{
public:
  Arena();
  Arena(size_t size);
  ~Arena();

  template <typename T>
  T *allocate(size_t count = 1);
  void deallocate();
  Snapshot snapshot();
  void rollback(const Snapshot &snapshot);

  void print() const;
  void print(unsigned char c) const;

private:
  unsigned char *memory_;
  size_t size_;
  size_t offset_;

  Arena(const Arena &other);
  Arena &operator=(const Arena &other);

  uintptr_t align(uintptr_t ptr, size_t alignment);
};

template <typename T>
T* Arena::allocate(size_t count) {
    const size_t size = sizeof(T) * count;
    if ((offset_ + size) > size_) {
        std::cerr << "Attempt to allocate " << size << " bytes in arena of size " << size_ << " with " << offset_ << " bytes already allocated" << std::endl;
        return NULL;
    }
    T* result = new (memory_ + offset_) T[count];
    offset_ += size;
    return result;
}
