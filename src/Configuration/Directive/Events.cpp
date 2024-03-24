#include "Blocks.hpp"

namespace configuration
{
  DirectiveEvents::DirectiveEvents()
    : DirectiveBlock() {}
  
  DirectiveEvents::DirectiveEvents(const Context& context)
    : DirectiveBlock(context) {}
  
  DirectiveEvents::DirectiveEvents(const DirectiveEvents& other)
    : DirectiveBlock(other) {}
  
  DirectiveEvents& DirectiveEvents::operator=(const DirectiveEvents& other)
  {
    if (this != &other)
      DirectiveBlock::operator=(other);
    return *this;
  }

  DirectiveEvents::~DirectiveEvents() {}

  Directive::Type DirectiveEvents::type() const
  {
    return Directive::kDirectiveEvents;
  }
} // namespace configuration
