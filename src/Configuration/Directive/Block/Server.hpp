#pragma once

#include "misc/Maybe.hpp"

#include "Configuration/Directive/Block.hpp"
#include "Configuration/Directive/Block/Location.hpp"

namespace directive
{
  class ServerBlock : public DirectiveBlock
  {
    public:
      ServerBlock();
      ServerBlock(const Context& context);
      ServerBlock(const ServerBlock& other);
      ServerBlock& operator=(const ServerBlock& other);
      virtual ~ServerBlock();

      virtual Type          type() const;

      Maybe<Locations>      locations();

    private:
      Maybe<Locations> locations_;
  };
} // namespace configuration
