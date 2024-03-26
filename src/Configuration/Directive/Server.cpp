#include "Blocks.hpp"

#include "misc/Maybe.hpp"

namespace configuration
{
  DirectiveServer::DirectiveServer()
    : DirectiveBlock(), locations_() {}

  DirectiveServer::DirectiveServer(const Context& context)
    : DirectiveBlock(context), locations_() {}
  
  DirectiveServer::DirectiveServer(const DirectiveServer& other)
    : DirectiveBlock(other), locations_() {}
  
  DirectiveServer& DirectiveServer::operator=(const DirectiveServer& other)
  {
    if (this != &other)
    {
      DirectiveBlock::operator=(other);
      locations_ = Nothing();
    }
    return *this;
  }

  DirectiveServer::~DirectiveServer() {}

  Directive::Type DirectiveServer::type() const
  {
    return Directive::kDirectiveServer;
  }

  Maybe<Locations> DirectiveServer::get_locations()
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
