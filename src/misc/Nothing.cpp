#include "Nothing.hpp"

Nothing::Nothing() {}

Nothing::Nothing(const Nothing& other) { (void) other; }

Nothing::~Nothing() {}

Nothing& Nothing::operator=(const Nothing& other)
{
  (void) other;
  return *this;
}

bool  Nothing::operator==(const Nothing& other) const
{
  (void) other;
  return true;
}
