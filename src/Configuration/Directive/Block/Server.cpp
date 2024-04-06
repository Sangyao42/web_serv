#include "Server.hpp"

#include "Configuration/Directive.hpp"
#include "Configuration/Directive/Block.hpp"
#include "Configuration/Directive/Block/Location.hpp"

namespace directive
{
  ServerBlock::ServerBlock()
    : DirectiveBlock() {}

  ServerBlock::ServerBlock(const Context& context)
    : DirectiveBlock(context) {}
  
  ServerBlock::ServerBlock(const ServerBlock& other)
    : DirectiveBlock(other) {}
  
  ServerBlock& ServerBlock::operator=(const ServerBlock& other)
  {
    if (this != &other)
    {
      DirectiveBlock::operator=(other);
    }
    return *this;
  }

  ServerBlock::~ServerBlock() {}

  Directive::Type ServerBlock::type() const
  {
    return Directive::kDirectiveServer;
  }

  Locations ServerBlock::locations() const
  {
    return directives_.equal_range(Directive::kDirectiveLocation);
  }
} // namespace configuration
