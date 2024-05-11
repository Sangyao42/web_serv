#include "Arena.hpp"

#include <cstddef>

namespace arena
{
  Arena permanent(1024);
  Arena temporary(1024);
} // namespace arena

Arena::Arena(size_t size)
  : memory_(::new unsigned char[size]), size_(size)
{
  fill_ = memory_;
}

Arena::~Arena() {
  delete[] memory_;
}

void Arena::clear() {
  fill_ = memory_;
}

ArenaSnapshot Arena::snapshot() {
  return fill_;
}

void Arena::rollback(ArenaSnapshot snapshot) {
  fill_ = snapshot;
}

uintptr_t Arena::align(uintptr_t ptr, size_t alignment) {
  return (ptr + alignment - 1) & ~(alignment - 1);
}
