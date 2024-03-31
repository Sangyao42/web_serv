#include "./Block.hpp"

#include <cassert>

#include "misc/Maybe.hpp"

#include "Configuration/Directive.hpp"

namespace directive
{

  /////////////////////////////////////////////
  ////////////   Directive Block   ////////////
  /////////////////////////////////////////////

  DirectiveBlock::DirectiveBlock()
    : Directive(), directives_(), parent_(NULL) {}

  DirectiveBlock::DirectiveBlock(const Context& context)
    : Directive(context), directives_(), parent_(NULL) {}

  DirectiveBlock::DirectiveBlock(const DirectiveBlock& other)
    : Directive(other), directives_(other.directives_), parent_(NULL)
  {
    for (Directives::iterator it = directives_.begin(); it != directives_.end(); ++it)
    {
      Directive* directive = it->second;
      if (directive->is_block())
      {
        static_cast<DirectiveBlock*>(directive)->set_parent(this);
      }
    }
  }

  DirectiveBlock& DirectiveBlock::operator=(const DirectiveBlock& other)
  {
    Directive::operator=(other);
    directives_ = other.directives_;
    return *this;
  }

  DirectiveBlock::~DirectiveBlock()
  {
    for (Directives::iterator it = directives_.begin(); it != directives_.end(); ++it)
    {
      delete it->second;
    }
  }

  bool DirectiveBlock::is_block() const
  {
    return true;
  }

  const Directives& DirectiveBlock::directives() const
  {
    return directives_;
  }

  DirectiveBlock* DirectiveBlock::parent()
  {
    return parent_;
  }

  const DirectiveBlock* DirectiveBlock::parent() const
  {
    return parent_;
  }

  void DirectiveBlock::set_parent(DirectiveBlock* parent)
  {
    parent_ = parent;
  }

  void DirectiveBlock::add_directive(Directive* directive)
  {
    assert(directive != NULL);
    directive->set_context(directives_.size());
    if (directive->is_block())
    {
      static_cast<DirectiveBlock*>(directive)->set_parent(this);
    }
    directives_.insert(std::make_pair(directive->type(), directive));
  }

  DirectivesRange DirectiveBlock::query_directive(Type type) const
  {
    return directives_.equal_range(type);
  }

  bool DirectiveRangeIsValid(const DirectivesRange& range)
  {
    return (range != DirectivesRange()) && (range.first != range.second);
  }
} // namespace configuration
