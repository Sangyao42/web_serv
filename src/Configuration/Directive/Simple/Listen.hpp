#pragma once

#include <vector>

#include "Configuration/Directive.hpp"
#include "Configuration/Directive/Socket.hpp"

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

      void                        add(const Socket& socket);
      const std::vector<Socket>&  get() const;

    private:
      std::vector<Socket>         sockets_;
  };
} // namespace configuration
