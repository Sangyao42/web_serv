#include "Simple.hpp"

#include <string>

namespace configuration
{
 DirectiveErrorPage::DirectiveErrorPage()
   : Directive(), path_() {}

  DirectiveErrorPage::DirectiveErrorPage(const Context& context)
    : Directive(context), path_() {}

  DirectiveErrorPage::DirectiveErrorPage(const DirectiveErrorPage& other)
    : Directive(other), path_(other.path_) {}

  DirectiveErrorPage& DirectiveErrorPage::operator=(const DirectiveErrorPage& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      path_ = other.path_;
    }
    return *this;
  }

  DirectiveErrorPage::~DirectiveErrorPage() {}

  bool DirectiveErrorPage::is_block() const
  {
    return false;
  }

  Directive::Type DirectiveErrorPage::type() const
  {
    return Directive::kDirectiveErrorPage;
  }

  void DirectiveErrorPage::set(const std::string& path)
  {
    path_ = path;
  }

  const std::string& DirectiveErrorPage::get() const
  {
    return path_;
  }
} // namespace configuration
