#include "Location.hpp"

#include <cassert>

#include <vector>
#include <utility>

#include "misc/Maybe.hpp"
#include "Configuration/Directive.hpp"
#include "Configuration/Directive/Block.hpp"

namespace directive
{
  //////////////////////////////////////////////////////
  ////////////   LocationBlockMatchResult   ////////////
  //////////////////////////////////////////////////////

  bool operator<(const LocationBlockMatchResult& lhs, const LocationBlockMatchResult& rhs)
  {
    if (lhs == LocationBlockMatchResult())
    {
      if (rhs == LocationBlockMatchResult())
        return false;
      else
        return true;
    }
    else if (rhs == LocationBlockMatchResult())
      return false;
    return lhs.second->match().length() < rhs.second->match().length();
  }

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

  LocationBlockMatchResult LocationBlock::best_match(const std::string& uri) const
  {
    if (!is_match(uri))
      return LocationBlockMatchResult(); // empty pair
    LocationBlockMatchResult  result = std::make_pair(LocationBlockPath(), this);

    result.first.push_back(this->index());
    if (locations_.is_ok())
    {
      const Locations& locations = locations_.value();
      // iterate over all paths in a location block
      for (Locations::first_type location_it = locations.first; location_it != locations.second; ++location_it)
      {
        assert(location_it->first == Directive::kDirectiveLocation);
        assert(location_it->second != NULL);
        const LocationBlock* location_block = static_cast<const LocationBlock*>(location_it->second);

        LocationBlockMatchResult  best_match = location_block->best_match(uri);
        // check if the best match is empty
        if (best_match != std::pair<LocationBlockPath, const LocationBlock*>())
        {
          const LocationBlock* location_block = best_match.second;
          if (result < best_match)
          {
            // update the best match
            const LocationBlockPath& location_path = best_match.first;
            result.first.insert(result.first.begin() + 1, location_path.begin(), location_path.end());
            result.second = location_block;
          }
        }
      }
    }
    return result;
  }
} // namespace configuration
