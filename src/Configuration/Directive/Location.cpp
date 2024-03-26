#include "Blocks.hpp"

#include "misc/Maybe.hpp"

namespace configuration
{
  DirectiveLocation::DirectiveLocation()
    : DirectiveBlock(), locations_() {}
  
  DirectiveLocation::DirectiveLocation(const Context& context)
    : DirectiveBlock(context), locations_() {}
  
  DirectiveLocation::DirectiveLocation(const DirectiveLocation& other)
    : DirectiveBlock(other), locations_() {}
  
  DirectiveLocation& DirectiveLocation::operator=(const DirectiveLocation& other)
  {
    if (this != &other)
    {
      DirectiveBlock::operator=(other);
      locations_ = Nothing();
    }
    return *this;
  }

  DirectiveLocation::~DirectiveLocation() {}

  Directive::Type DirectiveLocation::type() const
  {
    return Directive::kDirectiveLocation;
  }

  Maybe<Locations> DirectiveLocation::locations()
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
