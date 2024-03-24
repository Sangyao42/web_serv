#include "Simple.hpp"

namespace configuration
{
  DirectiveAllowMethods::DirectiveAllowMethods()
    : Directive(), accepted_methods_(DirectiveAllowMethods::kDefaultMethods) {}
  
  DirectiveAllowMethods::DirectiveAllowMethods(const Context& context)
    : Directive(context), accepted_methods_(DirectiveAllowMethods::kDefaultMethods) {}
  
  DirectiveAllowMethods::DirectiveAllowMethods(const DirectiveAllowMethods& other)
    : Directive(other), accepted_methods_(other.accepted_methods_) {}
  
  DirectiveAllowMethods& DirectiveAllowMethods::operator=(const DirectiveAllowMethods& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      accepted_methods_ = other.accepted_methods_;
    }
    return *this;
  }

  DirectiveAllowMethods::~DirectiveAllowMethods() {}

  bool DirectiveAllowMethods::is_block() const
  {
    return false;
  }

  Directive::Type DirectiveAllowMethods::type() const
  {
    return Directive::kDirectiveAllowMethods;
  }

  void DirectiveAllowMethods::set(int method)
  {
    accepted_methods_ = method;
  }

  const Methods DirectiveAllowMethods::get() const
  {
    return accepted_methods_;
  }
} // namespace configuration
