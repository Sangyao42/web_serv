#include "Simple.hpp"

#include <string>

namespace configuration
{
  DirectiveRedirect::DirectiveRedirect()
    : Directive(), path_(), is_permanent_(false) {}
  
  DirectiveRedirect::DirectiveRedirect(const Context& context)
    : Directive(context), path_(), is_permanent_(false) {}
  
  DirectiveRedirect::DirectiveRedirect(const DirectiveRedirect& other)
    : Directive(other), path_(other.path_), is_permanent_(other.is_permanent_) {}
  
  DirectiveRedirect& DirectiveRedirect::operator=(const DirectiveRedirect& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      path_ = other.path_;
      is_permanent_ = other.is_permanent_;
    }
    return *this;
  }

  DirectiveRedirect::~DirectiveRedirect() {}

  bool DirectiveRedirect::is_block() const
  {
    return false;
  }

  Directive::Type DirectiveRedirect::type() const
  {
    return Directive::kDirectiveRedirect;
  }

  void DirectiveRedirect::set(const std::string& path, bool is_permanent)
  {
    path_ = path;
    is_permanent_ = is_permanent;
  }

  const std::string& DirectiveRedirect::get_path() const
  {
    return path_;
  }

  bool DirectiveRedirect::is_permanent() const
  {
    return is_permanent_;
  }
} // namespace configuration
