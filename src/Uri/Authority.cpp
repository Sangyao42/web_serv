#include "Authority.hpp"

#include <sys/socket.h>
#include <cassert>
#include <cctype>

#include <bitset>
#include <algorithm>

namespace uri
{
  Host::Host()
    : type(IPV4), value(), serialized(0) {}

  Host::Host(const std::string& value)
    : type(IPV4), value(value), serialized(0)
  {
    recognize_type(value);
    serialize();
  }

  Host::Host(Type type, const std::string& value)
    : type(type), value(value), serialized(0)
  {
    serialize();
  }

  bool Host::operator==(const Host& other) const
  {
    return type == other.type && serialized == other.serialized;
  }

  void  Host::recognize_type(const std::string& value)
  {
    type = IPV4;
    if (value.find(':') != std::string::npos)
      type = IPV6;
    else
    {
      for (std::string::const_iterator it = value.begin(); it != value.end(); ++it)
      {
        if (std::isalpha(*it))
        {
          type = REGNAME;
          break;
        }
      }
    }
  }

  void  Host::serialize()
  {
    serialized = 0;
    switch (type)
    {
    case IPV4:
      {
        int octet = 0;
        for (std::string::const_iterator it = value.begin(); it != value.end(); ++it)
        {
          if (*it == '.')
          {
            serialized |= octet;
            serialized <<= 8;
            octet = 0;
          }
          else
          {
            octet = octet * 10 + (*it - '0');
          }
        }
      }
      break;
    case IPV6:
      {
        const int total_number_of_colons = std::count(value.begin(), value.end(), ':');
        assert(total_number_of_colons <= 7);
        int hextet = 0;
        for (std::string::const_iterator it = value.begin(); it != value.end(); ++it)
        {
          if (*it == ':')
          {
            serialized |= hextet;
            serialized <<= 16;
            hextet = 0;
            // handle the case of double colons
            if (((it + 1) != value.end()) && (*(it + 1) == ':'))
            {
              const int remaining_hextets = 8 - total_number_of_colons;
              serialized <<= (16 * remaining_hextets);
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
      break;
    case REGNAME:
      break;
    default:
      break;
    }
  }

  Authority::Authority()
    : host(Host::IPV4, "0.0.0.0"),
      port("80") {}

  Authority::Authority(const std::string& port)
    : host(Host::IPV4, "0.0.0.0"),
      port(port) {}

  Authority::Authority(const std::string& host, const std::string& port)
    : host(host),
      port(port) {}

  Authority& Authority::operator=(const Authority& other)
  {
    host = other.host;
    port = other.port;
    return *this;
  }

  Authority::~Authority() {}

  bool Authority::operator==(const Authority& other) const
  {
    return host == other.host && port == other.port;
  }

  unsigned int Authority::family() const
  {
    return host.type;
  }
} // namespace uri
