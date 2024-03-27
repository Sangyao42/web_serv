#include "Simple.hpp"

#include <vector>

namespace configuration
{
  DirectiveListen::DirectiveListen()
    : Directive(), sockets_() {}

  DirectiveListen::DirectiveListen(const Context& context)
    : Directive(context), sockets_() {}
  
  DirectiveListen::DirectiveListen(const DirectiveListen& other)
    : Directive(other), sockets_(other.sockets_) {}
  
  DirectiveListen& DirectiveListen::operator=(const DirectiveListen& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      sockets_ = other.sockets_;
    }
    return *this;
  }

  DirectiveListen::~DirectiveListen() {}

  bool DirectiveListen::is_block() const
  {
    return false;
  }

  Directive::Type DirectiveListen::type() const
  {
    return Directive::kDirectiveListen;
  }

  void DirectiveListen::add(const Socket& socket)
  {
    sockets_.push_back(socket);
  }

  const std::vector<Socket>& DirectiveListen::get() const
  {
    return sockets_;
  }
} // namespace configuration
