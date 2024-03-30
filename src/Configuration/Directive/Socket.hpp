#pragma once

#include <string>
#include <bitset>

namespace directive
{
  class Socket
  {
    public:
      Socket();
      Socket(const std::string& port);
      Socket(const std::string& ip_address, const std::string& port);
      Socket(const Socket& other);
      Socket& operator=(const Socket& other);
      ~Socket();

      bool  operator==(const Socket& other) const;

      const std::string&  ip_address() const;
      const std::string&  port() const;
      unsigned int        family() const;

      std::bitset<128>    ip_address_as_bits() const;

    private:
      std::string         ip_address_;
      std::string         port_;
      unsigned int        family_;
      std::bitset<128>    serialized_ip_address_;

      void                recognize_family();
      void                serialize_ip_address();
  };

} // namespace configuration
