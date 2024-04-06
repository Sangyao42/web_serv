#pragma once

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

      virtual Type  type() const;

      Locations     locations() const;
  };
} // namespace configuration
