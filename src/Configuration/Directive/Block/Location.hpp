#pragma once

#include <string>
#include <vector>
#include <utility>

#include "Configuration/Directive/Block.hpp"

namespace directive
{
  /**
   * @brief Locations points to a range of Directives that are of type
   * DirectiveLocation.
  */
  typedef DirectivesRange   Locations;

  class LocationBlock : public DirectiveBlock
  {
    public:
      LocationBlock();
      LocationBlock(const Context& context);
      LocationBlock(const LocationBlock& other);
      LocationBlock& operator=(const LocationBlock& other);
      virtual ~LocationBlock();

      bool  operator<(const LocationBlock& rhs) const;

      virtual Type          type() const;

      Locations             locations() const;
      const std::string&    match() const;
      void                  set(const std::string& match);

      bool                  is_match(const std::string& uri) const;

      const LocationBlock*  best_match(const std::string& uri) const;

    private:
      std::string           match_;
  };

} // namespace configuration
