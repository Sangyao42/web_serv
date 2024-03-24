#include "Simple.hpp"

#include <string>

#include "misc/Maybe.hpp"
#include "misc/Nothing.hpp"

namespace configuration
{
  DirectiveCgi::DirectiveCgi()
    : Directive(), extension_(), cgi_path_() {}
  
  DirectiveCgi::DirectiveCgi(const Context& context)
    : Directive(context), extension_(), cgi_path_() {}
  
  DirectiveCgi::DirectiveCgi(const DirectiveCgi& other)
    : Directive(other), extension_(other.extension_), cgi_path_(other.cgi_path_) {}
  
  DirectiveCgi& DirectiveCgi::operator=(const DirectiveCgi& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      extension_ = other.extension_;
      cgi_path_ = other.cgi_path_;
    }
    return *this;
  }

  DirectiveCgi::~DirectiveCgi() {}

  bool DirectiveCgi::is_block() const
  {
    return false;
  }

  Directive::Type DirectiveCgi::type() const
  {
    return Directive::kDirectiveCgi;
  }

  void DirectiveCgi::set(const std::string& extension, const std::string& cgi_path)
  {
    extension_ = extension;
    cgi_path_ = cgi_path;
  }

  Maybe<const std::string> DirectiveCgi::match(std::string extension) const
  {
    if (extension == extension_)
      return cgi_path_;
    return Nothing();
  }
} // namespace configuration
