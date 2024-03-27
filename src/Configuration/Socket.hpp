#pragma once

#include <string>

namespace configuration
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

      bool  operator<(const Socket& other) const;
      bool  operator==(const Socket& other) const;

      const std::string& ip_address() const;
      const std::string& port() const;
      unsigned int       family() const;

    private:
      std::string   ip_address_;
      std::string   port_;
      unsigned int  family_;

      void        recognize_family();
  };
} // namespace configuration
