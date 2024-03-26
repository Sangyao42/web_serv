#pragma once

#include <string>
#include <vector>

#include "Configuration/Socket.hpp"
#include "Configuration/Directive/Blocks.hpp"

class SocketConfiguration
{
  public:
    SocketConfiguration();
    SocketConfiguration(configuration::Socket socket);
    SocketConfiguration(configuration::Socket socket, configuration::DirectiveServer* server);
    SocketConfiguration(const SocketConfiguration& other);
    SocketConfiguration& operator=(const SocketConfiguration& other);
    ~SocketConfiguration();

    // So that it can be used in std::set and std::multiset
    bool  operator<(const SocketConfiguration& other) const;
    bool  operator==(const SocketConfiguration& other) const;

    const configuration::Socket& socket() const;

    const std::vector<configuration::DirectiveServer*>& servers() const;

    void  add_server(configuration::DirectiveServer* server);
    void  append(const SocketConfiguration& other);

  private:
    configuration::Socket                         socket_;
    std::vector<configuration::DirectiveServer*>  servers_;
};
