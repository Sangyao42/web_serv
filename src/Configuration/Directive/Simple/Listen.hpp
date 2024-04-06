#pragma once

#include <vector>

#include "Uri/Authority.hpp"
#include "Configuration/Directive.hpp"

namespace directive
{
  class Listen : public Directive
  {
    public:
      Listen();
      Listen(const Context& context);
      Listen(const Listen& other);
      Listen& operator=(const Listen& other);
      virtual ~Listen();

      virtual bool                is_block() const;
      virtual Type                type() const;

      void                        add(const uri::Authority& socket);
      const std::vector<uri::Authority>&  get() const;

    private:
      std::vector<uri::Authority> sockets_;
  };
} // namespace configuration
