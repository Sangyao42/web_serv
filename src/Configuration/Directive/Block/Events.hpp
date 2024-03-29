#pragma once

#include <string>

#include "misc/Maybe.hpp"
#include "Configuration/Directive/Block.hpp"

namespace directive
{
  class EventsBlock : public DirectiveBlock
  {
    public:
      EventsBlock();
      EventsBlock(const Context& context);
      EventsBlock(const EventsBlock& other);
      EventsBlock& operator=(const EventsBlock& other);
      virtual ~EventsBlock();

      virtual Type          type() const;

      Maybe<size_t>         worker_connections() const;
  };
} // namespace configuration
