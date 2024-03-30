#include "Socket.hpp"

#include <sys/socket.h>
#include <cassert>
#include <cctype>

#include <bitset>
#include <algorithm>

namespace directive
{
  Socket::Socket()
    : ip_address_("0.0.0.0"),
      port_("80"),
      family_(AF_INET),
      serialized_ip_address_(0) {}

  Socket::Socket(const std::string& port)
    : ip_address_("0.0.0.0"),
      port_(port),
      family_(AF_INET),
      serialized_ip_address_(0) {}

  Socket::Socket(const std::string& ip_address, const std::string& port)
    : ip_address_(ip_address),
      port_(port),
      family_(AF_UNSPEC),
      serialized_ip_address_(0)
  {
    recognize_family();
    serialize_ip_address();
  }

  Socket::Socket(const Socket& other)
    : ip_address_(other.ip_address_),
      port_(other.port_),
      family_(other.family_),
      serialized_ip_address_(other.serialized_ip_address_) {}

  Socket& Socket::operator=(const Socket& other)
  {
    if (this != &other)
    {
      ip_address_ = other.ip_address_;
      port_ = other.port_;
      family_ = other.family_;
      serialized_ip_address_ = other.serialized_ip_address_;
    }
    return *this;
  }

  Socket::~Socket() {}

  bool  Socket::operator==(const Socket& other) const
  {
    return serialized_ip_address_ == other.serialized_ip_address_ && port_ == other.port_ && family_ == other.family_;
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

  std::bitset<128>  Socket::ip_address_as_bits() const
  {
    return serialized_ip_address_;
  }

  void  Socket::recognize_family()
  {
    if (ip_address_.find(':') != std::string::npos)
      family_ = AF_INET6;
    else
      family_ = AF_INET;
  }

  void  Socket::serialize_ip_address()
  {
    assert(family_ == AF_INET || family_ == AF_INET6);
    serialized_ip_address_ = 0;
    if (family_ == AF_INET)
    {
      int octet = 0;
      for (std::string::const_iterator it = ip_address_.begin(); it != ip_address_.end(); ++it)
      {
        if (*it == '.')
        {
          serialized_ip_address_ |= octet;
          serialized_ip_address_ <<= 8;
          octet = 0;
        }
        else
        {
          octet = octet * 10 + (*it - '0');
        }
      }
    }
    else if (family_ == AF_INET6)
    {
      const int total_number_of_colons = std::count(ip_address_.begin(), ip_address_.end(), ':');
      assert(total_number_of_colons <= 7);
      int hextet = 0;
      for (std::string::const_iterator it = ip_address_.begin(); it != ip_address_.end(); ++it)
      {
        if (*it == ':')
        {
          serialized_ip_address_ |= hextet;
          serialized_ip_address_ <<= 16;
          hextet = 0;
          // handle the case of double colons
          if (((it + 1) != ip_address_.end()) && (*(it + 1) == ':'))
          {
            const int remaining_hextets = 8 - total_number_of_colons;
            serialized_ip_address_ <<= (16 * remaining_hextets);
            it++;
          }
        }
        else
        {
          char  reference_char = '0';
          if (*it >= 'a' && *it <= 'f')
            reference_char = 'a';
          else if (*it >= 'A' && *it <= 'F')
            reference_char = 'A';
          hextet = hextet * 16 + (*it - reference_char);
        }
      }
    }
  }

} // namespace configuration
