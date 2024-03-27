#include "Socket.hpp"

#include <sys/socket.h>

#include <bitset>

namespace configuration
{
  Socket::Socket()
    : ip_address_("0.0.0.0"), port_("80"), family_(AF_INET) {}

  Socket::Socket(const std::string& port)
    : ip_address_("0.0.0.0"), port_(port), family_(AF_INET) {}

  Socket::Socket(const std::string& ip_address, const std::string& port)
    : ip_address_(ip_address), port_(port), family_(AF_UNSPEC)
  {
    recognize_family();
  }

  Socket::Socket(const Socket& other)
    : ip_address_(other.ip_address_), port_(other.port_), family_(other.family_) {}

  Socket& Socket::operator=(const Socket& other)
  {
    if (this != &other)
    {
      ip_address_ = other.ip_address_;
      port_ = other.port_;
      family_ = other.family_;
    }
    return *this;
  }

  Socket::~Socket() {}

  bool  Socket::operator<(const Socket& other) const
  {
    if (ip_address_ == other.ip_address_)
      return port_ < other.port_;
    return ip_address_ < other.ip_address_;
  }

  bool  Socket::operator==(const Socket& other) const
  {
    return ip_address_ == other.ip_address_ && port_ == other.port_;
  }

  const std::string& Socket::ip_address() const
  {
    return ip_address_;
  }

  const std::string& Socket::port() const
  {
    return port_;
  }

  unsigned int Socket::family() const
  {
    return family_;
  }

  void  Socket::recognize_family()
  {
    if (ip_address_.find(':') != std::string::npos)
      family_ = AF_INET6;
    else
      family_ = AF_INET;
  }

} // namespace configuration
