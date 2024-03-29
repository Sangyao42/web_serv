#include "Blocks.hpp"

#include "Simples.hpp"

namespace directive
{
  EventsBlock::EventsBlock()
    : DirectiveBlock() {}
  
  EventsBlock::EventsBlock(const Context& context)
    : DirectiveBlock(context) {}
  
  EventsBlock::EventsBlock(const EventsBlock& other)
    : DirectiveBlock(other) {}
  
  EventsBlock& EventsBlock::operator=(const EventsBlock& other)
  {
    if (this != &other)
      DirectiveBlock::operator=(other);
    return *this;
  }

  EventsBlock::~EventsBlock() {}

  Directive::Type EventsBlock::type() const
  {
    return Directive::kDirectiveEvents;
  }

  Maybe<size_t> EventsBlock::worker_connections() const
  {
    DirectivesRange query_result = query_directive(Directive::kDirectiveWorkerConnections);
    if (query_result.first == query_result.second)
      return Nothing();
    return static_cast<WorkerConnections*>(query_result.first->second)->get();
  }
} // namespace configuration
