#pragma once

#include <string>

#include "misc/Maybe.hpp"
#include "Configuration/Directive.hpp"

namespace directive
{
  class ErrorPage : public Directive
  {
    public:
      ErrorPage();
      ErrorPage(const Context& context);
      ErrorPage(const ErrorPage& other);
      ErrorPage& operator=(const ErrorPage& other);
      virtual ~ErrorPage();

      virtual bool        is_block() const;
      virtual Type        type() const;
	  virtual void		  print(int) const;

      void                set(int error_code, const std::string& ErrorPage_path);

      int                 error_code() const;
      const std::string&  file_path() const;
      int                 get() const;

      Maybe<std::string>  match(int error_code) const;

    private:
      int           error_code_;
      std::string   file_path_;
  };
} // namespace configuration
