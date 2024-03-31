#pragma once

#include <map>
#include <utility>

#include "Configuration/Directive.hpp"

namespace directive
{
  /**
   * @brief Directives is a map of directive names to their corresponding
   * Directive objects. It is possible to have multiple entry for the same
   * directive name, in which case the order of insertion is preserved. Therefore
   * multimap is used.
  */
  typedef std::multimap<Directive::Type, Directive*> Directives;

  typedef std::pair<Directives::const_iterator, Directives::const_iterator> DirectivesRange;

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
      DirectiveBlock*         parent();
      const DirectiveBlock*   parent() const;
      void                    set_parent(DirectiveBlock* parent);

      virtual void            add_directive(Directive* directive);
      virtual DirectivesRange query_directive(Type type) const;

    protected:
      Directives              directives_;
      DirectiveBlock*         parent_;
  };

  bool  DirectiveRangeIsValid(const DirectivesRange& range);

} // namespace configuration
