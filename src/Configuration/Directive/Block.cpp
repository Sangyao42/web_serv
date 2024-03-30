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
    : Directive(), directives_() {}
  
  DirectiveBlock::DirectiveBlock(const Context& context)
    : Directive(context), directives_() {}
  
  DirectiveBlock::DirectiveBlock(const DirectiveBlock& other)
    : Directive(other), directives_(other.directives_) {}
  
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

  void DirectiveBlock::add_directive(Directive* directive)
  {
    assert(directive != NULL);
    directive->set_context(directives_.size());
    directives_.insert(std::make_pair(directive->type(), directive));
  }

  DirectivesRange DirectiveBlock::query_directive(Type type) const
  {
    return directives_.equal_range(type);
  }

  bool DirectiveRangeIsValid(const DirectivesRange& range)
  {
    return range.first != range.second;
  }
} // namespace configuration
