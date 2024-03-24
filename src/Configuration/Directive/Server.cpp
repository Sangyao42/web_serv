#include "Blocks.hpp"

#include "misc/Maybe.hpp"

namespace configuration
{
  DirectiveServer::DirectiveServer()
    : DirectiveBlock(), locations_() {}

  DirectiveServer::DirectiveServer(const Context& context)
    : DirectiveBlock(context), locations_() {}
  
  DirectiveServer::DirectiveServer(const DirectiveServer& other)
    : DirectiveBlock(other), locations_(other.locations_) {}
  
  DirectiveServer& DirectiveServer::operator=(const DirectiveServer& other)
  {
    if (this != &other)
    {
      DirectiveBlock::operator=(other);
      locations_ = other.locations_;
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
      locations_ = static_cast<const Directives>(directives_).equal_range(Directive::kDirectiveLocation);
    return locations_;
  }
} // namespace configuration
