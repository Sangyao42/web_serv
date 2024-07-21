#pragma once

#include <bitset>

#include "Configuration/Directive.hpp"

namespace directive
{
  typedef int Methods;
  enum Method
  {
    kMethodGet    = 1,
    kMethodPost   = 2,
    kMethodDelete = 4
  };

  class AllowMethods : public Directive
  {
    public:
      static const int kDefaultMethods = kMethodGet | kMethodPost;
      AllowMethods();
      AllowMethods(const Context& context);
      AllowMethods(const AllowMethods& other);
      AllowMethods& operator=(const AllowMethods& other);
      virtual ~AllowMethods();

      virtual bool  is_block() const;
      virtual Type  type() const;
	  virtual void  print(int) const;

      void          set(int method);
      Methods       get() const;

      bool          is_allowed(Method method) const;

    private:
      Methods       accepted_methods_;
  };
} // namespace configuration
