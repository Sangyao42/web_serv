#include "Location.hpp"

#include <cassert>

#include <vector>
#include <utility>

#include "Configuration/Directive.hpp"
#include "Configuration/Directive/Block.hpp"

namespace directive
{
  ///////////////////////////////////////////
  ////////////   LocationBlock   ////////////
  ///////////////////////////////////////////

  LocationBlock::LocationBlock()
    : DirectiveBlock(), match_() {}
  
  LocationBlock::LocationBlock(const Context& context)
    : DirectiveBlock(context), match_() {}
  
  LocationBlock::LocationBlock(const LocationBlock& other)
    : DirectiveBlock(other), match_(other.match_) {}
  
  LocationBlock& LocationBlock::operator=(const LocationBlock& other)
  {
    if (this != &other)
    {
      DirectiveBlock::operator=(other);
      match_ = other.match_;
    }
    return *this;
  }

  LocationBlock::~LocationBlock() {}

  bool LocationBlock::operator<(const LocationBlock& rhs) const
  {
    return this->match().length() < rhs.match().length();
  }

  Directive::Type LocationBlock::type() const
  {
    return Directive::kDirectiveLocation;
  }

  Locations LocationBlock::locations() const
  {
    return directives_.equal_range(Directive::kDirectiveLocation);
  }

  const std::string& LocationBlock::match() const
  {
    return match_;
  }

  void  LocationBlock::set(const std::string& match)
  {
    match_ = match;
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

    Locations range = locations();
    if (!DirectiveRangeIsValid(range))
      return result;
    // iterate over all paths in a location block
    for (Locations::first_type location_it = range.first; location_it != range.second; ++location_it)
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
    return result;
  }
} // namespace configuration
