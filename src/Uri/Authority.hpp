#pragma once

#include <sys/socket.h>

#include <string>
#include <bitset>

namespace uri
{
  struct Host
  {
    enum Type
    {
      REGNAME,
      IPV4 = AF_INET,
      IPV6 = AF_INET6
    }                   type;
    std::string         value;
    std::bitset<128>    serialized;
  
    Host();
    Host(const std::string& value);
    Host(Type type, const std::string& value);

    bool  operator==(const Host& other) const;
  
    void  serialize();
    void  recognize_type(const std::string& value);
  };

  struct Authority
  {
    Host        host;
    std::string port;

    Authority();
    Authority(const std::string& port);
    Authority(const std::string& host, const std::string& port);
    Authority& operator=(const Authority& other);
    ~Authority();

    bool  operator==(const Authority& other) const;
  
    unsigned int  family() const;
  };
} // namespace uri
