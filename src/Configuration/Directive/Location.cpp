#include "Blocks.hpp"

#include "misc/Maybe.hpp"

namespace configuration
{
  DirectiveLocation::DirectiveLocation()
    : DirectiveBlock(), locations_() {}
  
  DirectiveLocation::DirectiveLocation(const Context& context)
    : DirectiveBlock(context), locations_() {}
  
  DirectiveLocation::DirectiveLocation(const DirectiveLocation& other)
    : DirectiveBlock(other), locations_(other.locations_) {}
  
  DirectiveLocation& DirectiveLocation::operator=(const DirectiveLocation& other)
  {
    if (this != &other)
    {
      DirectiveBlock::operator=(other);
      locations_ = other.locations_;
    }
    return *this;
  }

  DirectiveLocation::~DirectiveLocation() {}

  Directive::Type DirectiveLocation::type() const
  {
    return Directive::kDirectiveLocation;
  }

  Maybe<Locations> DirectiveLocation::get_locations()
  {
    if (!locations_.is_ok())
      locations_ = static_cast<const Directives>(directives_).equal_range(Directive::kDirectiveLocation);
    return locations_;
  }
} // namespace configuration
