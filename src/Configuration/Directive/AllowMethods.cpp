#include "Simples.hpp"

#include <cassert>

namespace directive
{
  AllowMethods::AllowMethods()
    : Directive(), accepted_methods_(AllowMethods::kDefaultMethods) {}
  
  AllowMethods::AllowMethods(const Context& context)
    : Directive(context), accepted_methods_(AllowMethods::kDefaultMethods) {}
  
  AllowMethods::AllowMethods(const AllowMethods& other)
    : Directive(other), accepted_methods_(other.accepted_methods_) {}
  
  AllowMethods& AllowMethods::operator=(const AllowMethods& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      accepted_methods_ = other.accepted_methods_;
    }
    return *this;
  }

  AllowMethods::~AllowMethods() {}

  bool AllowMethods::is_block() const
  {
    return false;
  }

  Directive::Type AllowMethods::type() const
  {
    return Directive::kDirectiveAllowMethods;
  }

  void AllowMethods::set(int method)
  {
    assert(method >= 0 && method < 8);
    accepted_methods_ = method;
  }

  Methods AllowMethods::get() const
  {
    return accepted_methods_;
  }

  bool  AllowMethods::is_allowed(Method method) const
  {
    return (accepted_methods_ & static_cast<std::bitset<3> >(method)) != 0;
  }
} // namespace configuration
