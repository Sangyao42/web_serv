#include "Server.hpp"

#include "misc/Maybe.hpp"
#include "Configuration/Directive.hpp"
#include "Configuration/Directive/Block.hpp"
#include "Configuration/Directive/Block/Location.hpp"

namespace directive
{
  ServerBlock::ServerBlock()
    : DirectiveBlock(), locations_() {}

  ServerBlock::ServerBlock(const Context& context)
    : DirectiveBlock(context), locations_() {}
  
  ServerBlock::ServerBlock(const ServerBlock& other)
    : DirectiveBlock(other), locations_() {}
  
  ServerBlock& ServerBlock::operator=(const ServerBlock& other)
  {
    if (this != &other)
    {
      DirectiveBlock::operator=(other);
      locations_ = Nothing();
    }
    return *this;
  }

  ServerBlock::~ServerBlock() {}

  Directive::Type ServerBlock::type() const
  {
    return Directive::kDirectiveServer;
  }

  Maybe<Locations> ServerBlock::locations()
  {
    if (!locations_.is_ok())
    {
      Locations locations = static_cast<const Directives>(directives_).equal_range(Directive::kDirectiveLocation);
      if (locations.first != locations.second)
        locations_ = locations;
    }
    return locations_;
  }
} // namespace configuration
