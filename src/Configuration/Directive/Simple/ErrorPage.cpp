#include "ErrorPage.hpp"

#include <string>

#include "misc/Maybe.hpp"
#include "misc/Nothing.hpp"
#include "Configuration/Directive.hpp"

namespace directive
{
  ErrorPage::ErrorPage()
    : Directive(), error_code_(), url_() {}
  
  ErrorPage::ErrorPage(const Context& context)
    : Directive(context), error_code_(), url_() {}
  
  ErrorPage::ErrorPage(const ErrorPage& other)
    : Directive(other), error_code_(other.error_code_), url_(other.url_) {}
  
  ErrorPage& ErrorPage::operator=(const ErrorPage& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      error_code_ = other.error_code_;
      url_ = other.url_;
    }
    return *this;
  }

  ErrorPage::~ErrorPage() {}

  bool ErrorPage::is_block() const
  {
    return false;
  }

  Directive::Type ErrorPage::type() const
  {
    return Directive::kDirectiveErrorPage;
  }

  void ErrorPage::set(int error_code, const std::string& url)
  {
    error_code_ = error_code;
    url_ = url;
  }


  int ErrorPage::error_code() const
  {
    return error_code_;
  }

  const std::string& ErrorPage::url() const
  {
    return url_;
  }

  int ErrorPage::get() const
  {
    return error_code_;
  }

  Maybe<std::string> ErrorPage::match(int error_code) const
  {
    if (error_code == error_code_)
      return url_;
    return Nothing();
  }
} // namespace configuration
