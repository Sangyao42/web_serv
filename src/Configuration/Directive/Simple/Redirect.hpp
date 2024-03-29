#pragma once

#include <string>

#include "Configuration/Directive.hpp"

namespace directive
{
  class Redirect : public Directive
  {
    public:
      Redirect();
      Redirect(const Context& context);
      Redirect(const Redirect& other);
      Redirect& operator=(const Redirect& other);
      virtual ~Redirect();

      virtual bool        is_block() const;
      virtual Type        type() const;

      void                set(const std::string& path, bool is_permanent);
      const std::string&  get_path() const;
      bool                is_permanent() const;

    private:
      std::string       path_;
      bool              is_permanent_;
  };
} // namespace configuration
