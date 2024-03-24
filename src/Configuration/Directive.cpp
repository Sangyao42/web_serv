#include "Directive.hpp"

#include <utility>
#include <cassert>

namespace configuration
{
  ///////////////////////////////////////////////
  ////////////   Directive Context   ////////////
  ///////////////////////////////////////////////

  Directive::Context::Context()
    : index(0) {}
  
  Directive::Context::Context(int index)
    : index(index) {}
  
  bool Directive::Context::operator==(const Context& context) const
  {
    return index == context.index;
  }
  
  ///////////////////////////////////////
  ////////////   Directive   ////////////
  ///////////////////////////////////////
  
  Directive::Directive()
    : context_() {}
  
  Directive::Directive(const Context& context)
    : context_(context) {}
  
  Directive::Directive(const Directive& other)
    : context_(other.context_) {}
  
  Directive& Directive::operator=(const Directive& other)
  {
    context_ = other.context_;
    return *this;
  }

  Directive::~Directive() {}

  const Directive::Context& Directive::context() const
  {
    return context_;
  }

  void  Directive::set_context(const Context& context)
  {
    context_ = context;
  }

  int Directive::index() const
  {
    return context_.index;
  }

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

  DirectiveBlock::~DirectiveBlock() {}

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

  DirectivesRange DirectiveBlock::get_directive(Type type) const
  {
    return directives_.equal_range(type);
  }
} // namespace configuration
