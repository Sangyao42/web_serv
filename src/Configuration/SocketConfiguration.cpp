#include "SocketConfiguration.hpp"

#include <string>
#include <vector>

SocketConfiguration::SocketConfiguration()
  : socket_(), servers_() {}

SocketConfiguration::SocketConfiguration(configuration::Socket socket)
  : socket_(socket), servers_() {}

SocketConfiguration::SocketConfiguration(configuration::Socket socket, configuration::DirectiveServer* server)
  : socket_(socket), servers_()
{
  servers_.push_back(server);
}

SocketConfiguration::SocketConfiguration(const SocketConfiguration& other)
  : socket_(other.socket_), servers_(other.servers_) {}

SocketConfiguration& SocketConfiguration::operator=(const SocketConfiguration& other)
{
  if (this != &other)
  {
    socket_ = other.socket_;
    servers_ = other.servers_;
  }
  return *this;
}

SocketConfiguration::~SocketConfiguration() {}

bool  SocketConfiguration::operator<(const SocketConfiguration& other) const
{
  return socket_ < other.socket_;
}

bool  SocketConfiguration::operator==(const SocketConfiguration& other) const
{
  return socket_ == other.socket_;
}

const configuration::Socket& SocketConfiguration::socket() const
{
  return socket_;
}

const std::vector<configuration::DirectiveServer*>& SocketConfiguration::servers() const
{
  return servers_;
}

void SocketConfiguration::add_server(configuration::DirectiveServer* server)
{
  servers_.push_back(server);
}

void  SocketConfiguration::append(const SocketConfiguration& other)
{
  servers_.insert(servers_.end(), other.servers_.begin(), other.servers_.end());
}
