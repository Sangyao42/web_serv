#pragma once

#include <string>

#include "misc/Maybe.hpp"
#include "Configuration/Directive/Block.hpp"

namespace directive
{
  /**
   * @brief Locations points to a range of Directives that are of type
   * DirectiveLocation.
  */
  typedef DirectivesRange Locations;

  class LocationBlock : public DirectiveBlock
  {
    public:
      LocationBlock();
      LocationBlock(const Context& context);
      LocationBlock(const LocationBlock& other);
      LocationBlock& operator=(const LocationBlock& other);
      virtual ~LocationBlock();

      virtual Type          type() const;

      Maybe<Locations>      locations();
      DirectiveBlock*       parent();
      const DirectiveBlock* parent() const;
      void                  set_parent(DirectiveBlock* parent);
      bool                  is_match(const std::string& uri) const;

    private:
      Maybe<Locations>      locations_;
      DirectiveBlock*       parent_;
      std::string           match_;
  };
} // namespace configuration
