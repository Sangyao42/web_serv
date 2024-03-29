#include "Redirect.hpp"

#include <string>

#include "Configuration/Directive.hpp"

namespace directive
{
  Redirect::Redirect()
    : Directive(), path_(), is_permanent_(false) {}
  
  Redirect::Redirect(const Context& context)
    : Directive(context), path_(), is_permanent_(false) {}
  
  Redirect::Redirect(const Redirect& other)
    : Directive(other), path_(other.path_), is_permanent_(other.is_permanent_) {}
  
  Redirect& Redirect::operator=(const Redirect& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      path_ = other.path_;
      is_permanent_ = other.is_permanent_;
    }
    return *this;
  }

  Redirect::~Redirect() {}

  bool Redirect::is_block() const
  {
    return false;
  }

  Directive::Type Redirect::type() const
  {
    return Directive::kDirectiveRedirect;
  }

  void Redirect::set(const std::string& path, bool is_permanent)
  {
    path_ = path;
    is_permanent_ = is_permanent;
  }

  const std::string& Redirect::get_path() const
  {
    return path_;
  }

  bool Redirect::is_permanent() const
  {
    return is_permanent_;
  }
} // namespace configuration
