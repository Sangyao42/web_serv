#include "Return.hpp"

#include <string>

#include "Configuration/Directive.hpp"

namespace directive
{
  Return::Return()
    : Directive(), path_(), status_code_(false) {}
  
  Return::Return(const Context& context)
    : Directive(context), path_(), status_code_(false) {}
  
  Return::Return(const Return& other)
    : Directive(other), path_(other.path_), status_code_(other.status_code_) {}
  
  Return& Return::operator=(const Return& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      path_ = other.path_;
      status_code_ = other.status_code_;
    }
    return *this;
  }

  Return::~Return() {}

  bool Return::is_block() const
  {
    return false;
  }

  Directive::Type Return::type() const
  {
    return Directive::kDirectiveReturn;
  }

  void Return::set(const std::string& path, int status_code)
  {
    path_ = path;
    status_code_ = status_code;
  }

  int Return::get() const
  {
    return status_code_;
  }

  const std::string& Return::get_path() const
  {
    return path_;
  }

  int Return::status_code() const
  {
    return status_code_;
  }
} // namespace configuration
