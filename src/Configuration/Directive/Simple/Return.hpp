#pragma once

#include <string>

#include "Configuration/Directive.hpp"

namespace directive
{
  class Return : public Directive
  {
    public:
      Return();
      Return(const Context& context);
      Return(const Return& other);
      Return& operator=(const Return& other);
      virtual ~Return();

      virtual bool        is_block() const;
      virtual Type        type() const;
	  virtual void		  print(int) const;

      void                set(const std::string& path, int status_code);
      int                 get() const;
      const std::string&  get_path() const;
      int                 status_code() const;

    private:
      std::string         path_;
      int                 status_code_;
  };
} // namespace configuration
