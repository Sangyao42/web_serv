#include "Listen.hpp"

#include <vector>

#include "Uri/Authority.hpp"
#include "Configuration/Directive.hpp"

namespace directive
{
  Listen::Listen()
    : Directive(), sockets_() {}

  Listen::Listen(const Context& context)
    : Directive(context), sockets_() {}
  
  Listen::Listen(const Listen& other)
    : Directive(other), sockets_(other.sockets_) {}
  
  Listen& Listen::operator=(const Listen& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      sockets_ = other.sockets_;
    }
    return *this;
  }

  Listen::~Listen() {}

  bool Listen::is_block() const
  {
    return false;
  }

  Directive::Type Listen::type() const
  {
    return Directive::kDirectiveListen;
  }

  void Listen::add(const uri::Authority& socket)
  {
    sockets_.push_back(socket);
  }

  const std::vector<uri::Authority>& Listen::get() const
  {
    return sockets_;
  }
} // namespace configuration
