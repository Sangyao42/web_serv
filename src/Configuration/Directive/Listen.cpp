#include "Simple.hpp"

#include <vector>

namespace configuration
{
  DirectiveListen::DirectiveListen()
    : Directive(), ip_addresses_() {}

  DirectiveListen::DirectiveListen(const Context& context)
    : Directive(context), ip_addresses_() {}
  
  DirectiveListen::DirectiveListen(const DirectiveListen& other)
    : Directive(other), ip_addresses_(other.ip_addresses_) {}
  
  DirectiveListen& DirectiveListen::operator=(const DirectiveListen& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      ip_addresses_ = other.ip_addresses_;
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

  void DirectiveListen::add(const IpAddress& ip_address)
  {
    ip_addresses_.push_back(ip_address);
  }

  const std::vector<DirectiveListen::IpAddress>& DirectiveListen::get() const
  {
    return ip_addresses_;
  }
} // namespace configuration
