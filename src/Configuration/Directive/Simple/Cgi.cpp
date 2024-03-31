#include "Cgi.hpp"

#include <string>

#include "misc/Maybe.hpp"
#include "misc/Nothing.hpp"
#include "Configuration/Directive.hpp"

namespace directive
{
  Cgi::Cgi()
    : Directive(), extension_(), cgi_path_() {}
  
  Cgi::Cgi(const Context& context)
    : Directive(context), extension_(), cgi_path_() {}
  
  Cgi::Cgi(const Cgi& other)
    : Directive(other), extension_(other.extension_), cgi_path_(other.cgi_path_) {}
  
  Cgi& Cgi::operator=(const Cgi& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      extension_ = other.extension_;
      cgi_path_ = other.cgi_path_;
    }
    return *this;
  }

  Cgi::~Cgi() {}

  bool Cgi::is_block() const
  {
    return false;
  }

  Directive::Type Cgi::type() const
  {
    return Directive::kDirectiveCgi;
  }

  void Cgi::set(const std::string& extension, const std::string& cgi_path)
  {
    extension_ = extension;
    cgi_path_ = cgi_path;
  }

  const std::string& Cgi::extension() const
  {
    return extension_;
  }

  const std::string& Cgi::cgi_path() const
  {
    return cgi_path_;
  }

  const std::string& Cgi::get() const
  {
    return extension_;
  }

  Maybe<std::string> Cgi::match(std::string extension) const
  {
    if (extension == extension_)
      return cgi_path_;
    return Nothing();
  }
} // namespace configuration
