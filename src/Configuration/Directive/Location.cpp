#include "Blocks.hpp"

#include "misc/Maybe.hpp"

namespace directive
{
  LocationBlock::LocationBlock()
    : DirectiveBlock(), locations_() {}
  
  LocationBlock::LocationBlock(const Context& context)
    : DirectiveBlock(context), locations_() {}
  
  LocationBlock::LocationBlock(const LocationBlock& other)
    : DirectiveBlock(other), locations_() {}
  
  LocationBlock& LocationBlock::operator=(const LocationBlock& other)
  {
    if (this != &other)
    {
      DirectiveBlock::operator=(other);
      locations_ = Nothing();
    }
    return *this;
  }

  LocationBlock::~LocationBlock() {}

  Directive::Type LocationBlock::type() const
  {
    return Directive::kDirectiveLocation;
  }

  Maybe<Locations> LocationBlock::locations()
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
