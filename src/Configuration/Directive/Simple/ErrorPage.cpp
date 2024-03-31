#include "ErrorPage.hpp"

#include <string>

#include "misc/Maybe.hpp"
#include "misc/Nothing.hpp"
#include "Configuration/Directive.hpp"

namespace directive
{
  ErrorPage::ErrorPage()
    : Directive(), error_code_(), file_path_() {}
  
  ErrorPage::ErrorPage(const Context& context)
    : Directive(context), error_code_(), file_path_() {}
  
  ErrorPage::ErrorPage(const ErrorPage& other)
    : Directive(other), error_code_(other.error_code_), file_path_(other.file_path_) {}
  
  ErrorPage& ErrorPage::operator=(const ErrorPage& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      error_code_ = other.error_code_;
      file_path_ = other.file_path_;
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

  void ErrorPage::set(int error_code, const std::string& file_path)
  {
    error_code_ = error_code;
    file_path_ = file_path;
  }


  int ErrorPage::error_code() const
  {
    return error_code_;
  }

  const std::string& ErrorPage::file_path() const
  {
    return file_path_;
  }

  int ErrorPage::get() const
  {
    return error_code_;
  }

  Maybe<std::string> ErrorPage::match(int error_code) const
  {
    if (error_code == error_code_)
      return file_path_;
    return Nothing();
  }
} // namespace configuration
