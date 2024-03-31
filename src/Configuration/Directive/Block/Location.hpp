#pragma once

#include <string>
#include <vector>
#include <utility>

#include "misc/Maybe.hpp"
#include "Configuration/Directive/Block.hpp"

namespace directive
{
  /**
   * @brief Locations points to a range of Directives that are of type
   * DirectiveLocation.
  */
  typedef DirectivesRange   Locations;
  typedef std::vector<int>  LocationBlockPath;
  class LocationBlock;
  typedef std::pair<LocationBlockPath, const LocationBlock*> LocationBlockMatchResult;

  bool  operator<(const LocationBlockMatchResult& lhs, const LocationBlockMatchResult& rhs);

  class LocationBlock : public DirectiveBlock
  {
    public:
      LocationBlock();
      LocationBlock(const Context& context);
      LocationBlock(const LocationBlock& other);
      LocationBlock& operator=(const LocationBlock& other);
      virtual ~LocationBlock();

      virtual Type              type() const;

      Maybe<Locations>          locations();
      const std::string&        match() const;

      bool                      is_match(const std::string& uri) const;

      LocationBlockMatchResult  best_match(const std::string& uri) const;

    private:
      Maybe<Locations>          locations_;
      std::string               match_;
  };

} // namespace configuration
