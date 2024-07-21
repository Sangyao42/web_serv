#include "Directive.hpp"
#include <iostream>
#include <cstring>

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

void	PrintIndentation(int indentation)
{
  static char spaces[256];
  static bool is_initialized = false;

  if (!is_initialized)
  {
    is_initialized = true;
    memset(spaces, ' ', 256);
  }
  if (indentation > 256)
  {
    indentation = 256;
  }
  else if (indentation < 0)
  {
    indentation = 0;
  }
  std::cout << std::string(spaces, indentation);
}

