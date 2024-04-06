#pragma once

#include <vector>

#include "Configuration/Directive/Block.hpp"
#include "Configuration/Directive/Block/Http.hpp"
#include "Configuration/Directive/Block/Events.hpp"

namespace directive
{
  class MainBlock : public DirectiveBlock
  {
    public:
      MainBlock();
      MainBlock(const Context& context);
      MainBlock(const MainBlock& other);
      MainBlock& operator=(const MainBlock& other);
      virtual ~MainBlock();

      virtual Type        type() const;

      virtual void        add_directive(Directive* directive);

      HttpBlock&          http();
      const HttpBlock&    http() const;
      EventsBlock&        events();
      const EventsBlock&  events() const;

    private:
      HttpBlock*          http_;
      EventsBlock*        events_;
  };
} // namespace directive
