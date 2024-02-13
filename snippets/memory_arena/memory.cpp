#include "memory.hpp"

#include <cstddef>
#include <cstdint>

Snapshot::Snapshot()
    : arena_(NULL), offset_(0) {}

Snapshot::Snapshot(Arena* arena, size_t offset)
    : arena_(arena), offset_(offset) {}

Snapshot::Snapshot(const Snapshot& other)
    : arena_(other.arena_), offset_(other.offset_) {}

Snapshot::~Snapshot() {}

Snapshot& Snapshot::operator=(const Snapshot& other) {
    arena_ = other.arena_;
    offset_ = other.offset_;
    return *this;
}

void Snapshot::rollback() {
    arena_->rollback(*this);
}

size_t Snapshot::offset() const {
    return offset_;
}

Arena::Arena()
    : memory_(NULL), offset_(0) {}

Arena::Arena(size_t size)
    : memory_(new unsigned char[size]), size_(size), offset_(0) {}

Arena::~Arena() {
    delete memory_;
}

void Arena::deallocate() {
    offset_ = 0;
}

Snapshot Arena::snapshot() {
    return Snapshot(this, offset_);
}

void Arena::rollback(const Snapshot& snapshot) {
    offset_ = snapshot.offset();
}

void  Arena::print() const {
  unsigned char *memory = memory_;
  std::cout << "Arena of size " << size_ << " with " << offset_ << " bytes allocated:" << std::endl;
  for (size_t i = 0; i < size_; i++)
  {
    print(*memory);
    if (i == offset_)
      std::cout << "\033[1;32m" << "<" << "\033[0m";
    memory++;
  }
  std::cout << std::endl;
}

void Arena::print(unsigned char c) const
{
  if (c == ' ')
    std::cout << "\033[1;33m" << "." << "\033[0m";
  else if (std::isprint(c))
    std::cout << c;
  else
    std::cout << "\033[1;31m" << static_cast<unsigned char>(c + 33) << "\033[0m";
}

uintptr_t Arena::align(uintptr_t ptr, size_t alignment) {
    return (ptr + alignment - 1) & ~(alignment - 1);
}
