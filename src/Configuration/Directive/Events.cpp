#include "Blocks.hpp"

#include "Simple.hpp"

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

  Maybe<size_t> DirectiveEvents::worker_connections() const
  {
    DirectivesRange query_result = query_directive(Directive::kDirectiveWorkerConnections);
    if (query_result.first == query_result.second)
      return Nothing();
    return static_cast<DirectiveWorkerConnections*>(query_result.first->second)->get();
  }
} // namespace configuration
