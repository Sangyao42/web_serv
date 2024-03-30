#include "Location.hpp"

#include "misc/Maybe.hpp"
#include "Configuration/Directive.hpp"
#include "Configuration/Directive/Block.hpp"

namespace directive
{
  LocationBlock::LocationBlock()
    : DirectiveBlock(), locations_(), parent_(NULL), match_() {}
  
  LocationBlock::LocationBlock(const Context& context)
    : DirectiveBlock(context), locations_(), parent_(NULL), match_() {}
  
  LocationBlock::LocationBlock(const LocationBlock& other)
    : DirectiveBlock(other), locations_(), parent_(NULL), match_(other.match_) {}
  
  LocationBlock& LocationBlock::operator=(const LocationBlock& other)
  {
    if (this != &other)
    {
      DirectiveBlock::operator=(other);
      locations_ = Nothing();
      match_ = other.match_;
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

  DirectiveBlock* LocationBlock::parent()
  {
    return parent_;
  }

  const DirectiveBlock* LocationBlock::parent() const
  {
    return parent_;
  }

  void LocationBlock::set_parent(DirectiveBlock* parent)
  {
    parent_ = parent;
  }

  bool LocationBlock::is_match(const std::string& uri) const
  {
    return uri.find(match_) == 0;
  }
} // namespace configuration
