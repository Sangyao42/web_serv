#include "Directive.hpp"

#include <utility>
#include <cassert>

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
