#include "Simple.hpp"

#include <cassert>

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
    assert(method >= 0 && method < 8);
    accepted_methods_ = method;
  }

  Methods DirectiveAllowMethods::get() const
  {
    return accepted_methods_;
  }

  bool  DirectiveAllowMethods::is_allowed(Method method) const
  {
    return (accepted_methods_ & static_cast<std::bitset<3> >(method)) != 0;
  }
} // namespace configuration
