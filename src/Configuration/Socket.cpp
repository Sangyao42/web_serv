#include "Socket.hpp"

#include <sys/socket.h>

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
    simplify_ipv6();
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

  u_int16_t Socket::family() const
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

  void  Socket::simplify_ipv6()
  {
    if (family_ != AF_INET6)
      return ;
    std::string simplified_ip_address;

    bool  found_value = false;
    int   empty_octets_amount = 0;

    for (std::string::iterator it = ip_address_.begin(); it != ip_address_.end(); ++it)
    {
      if (*it == '0')
      {
        if (found_value)
          simplified_ip_address.push_back('0');
      }
      else if (*it == ':')
      {
        if (found_value)
          empty_octets_amount = 0;
        if (empty_octets_amount < 2)
          simplified_ip_address.push_back(':');
        ++empty_octets_amount;
        found_value = false;
      }
      else
      {
        found_value = true;
        empty_octets_amount = 0;
        simplified_ip_address.push_back(*it);
      }
    }
    ip_address_ = simplified_ip_address;
  }
} // namespace configuration
