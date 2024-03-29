#pragma once

#include <map>
#include <vector>
#include <utility>

#include "misc/Maybe.hpp"

#include "Configuration/Directive.hpp"

namespace directive
{
  class ServerBlock;
  class LocationBlock;

  /**
   * @brief Directives is a map of directive names to their corresponding
   * Directive objects. It is possible to have multiple entry for the same
   * directive name, in which case the order of insertion is preserved. Therefore
   * multimap is used.
  */
  typedef std::multimap<Directive::Type, Directive*> Directives;

  typedef std::pair<Directives::const_iterator, Directives::const_iterator> DirectivesRange;

  /**
   * @brief Servers points to a range of Directives that are of type
   * DirectiveServer.
  */
  typedef DirectivesRange Servers;

  /**
   * @brief Locations points to a range of Directives that are of type
   * DirectiveLocation.
  */
  typedef DirectivesRange Locations;

  class DirectiveBlock : public Directive
  {
    public:
      DirectiveBlock();
      DirectiveBlock(const Context& context);
      DirectiveBlock(const DirectiveBlock& other);
      DirectiveBlock& operator=(const DirectiveBlock& other);
      virtual ~DirectiveBlock();

      virtual bool            is_block() const;
      virtual Type            type() const = 0;

      virtual void            add_directive(Directive* directive);
      virtual DirectivesRange query_directive(Type type) const;

    protected:
      Directives directives_;
  };

} // namespace configuration
