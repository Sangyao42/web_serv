#include "Location.hpp"

#include <cassert>

#include <vector>
#include <utility>

#include "misc/Maybe.hpp"
#include "Configuration/Directive.hpp"
#include "Configuration/Directive/Block.hpp"

namespace directive
{
  ///////////////////////////////////////////
  ////////////   LocationBlock   ////////////
  ///////////////////////////////////////////

  LocationBlock::LocationBlock()
    : DirectiveBlock(), locations_(), match_() {}
  
  LocationBlock::LocationBlock(const Context& context)
    : DirectiveBlock(context), locations_(), match_() {}
  
  LocationBlock::LocationBlock(const LocationBlock& other)
    : DirectiveBlock(other), locations_(), match_(other.match_) {}
  
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

  const std::string& LocationBlock::match() const
  {
    return match_;
  }

  bool  LocationBlock::is_match(const std::string& uri) const
  {
    return uri.find(match_) == 0;
  }

  const LocationBlock* LocationBlock::best_match(const std::string& uri) const
  {
    if (!is_match(uri))
      return NULL;
    const LocationBlock*  result = this;

    if (locations_.is_ok())
    {
      const Locations& locations = locations_.value();
      // iterate over all paths in a location block
      for (Locations::first_type location_it = locations.first; location_it != locations.second; ++location_it)
      {
        assert(location_it->first == Directive::kDirectiveLocation);
        assert(location_it->second != NULL);
        const LocationBlock* location_block = static_cast<const LocationBlock*>(location_it->second);

        const LocationBlock*  best_match = location_block->best_match(uri);
        if (best_match != NULL)
        {
          if (*result < *best_match)
            result = location_block;
        }
      }
    }
    return result;
  }

  //////////////////////////////////////////////////////
  ////////////   LocationBlockMatchResult   ////////////
  //////////////////////////////////////////////////////

  bool operator<(const LocationBlock& lhs, const LocationBlock& rhs)
  {
    return lhs.match().length() < rhs.match().length();
  }
} // namespace configuration
