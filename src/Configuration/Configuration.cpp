#include "Configuration.hpp"

#include <cassert>
#include <vector>

#include "SocketConfiguration.hpp"
#include "misc/Maybe.hpp"

Configuration ws_configuration;

Configuration::Configuration()
  : http_(), events_(), directives_() {}

Configuration::Configuration(const Configuration& other)
  : http_(other.http_), events_(other.events_), directives_(other.directives_) {}

Configuration& Configuration::operator=(const Configuration& other)
{
  if (this != &other)
  {
    http_ = other.http_;
    events_ = other.events_;
    directives_ = other.directives_;
  }
  return *this;
}

Configuration::~Configuration() {}

configuration::DirectiveHttp& Configuration::http_block()
{
  return http_;
}

const configuration::DirectiveHttp& Configuration::http_block() const
{
  return http_;
}

configuration::DirectiveEvents& Configuration::events_block()
{
  return events_;
}

const configuration::DirectiveEvents& Configuration::events_block() const
{
  return events_;
}

configuration::Directives& Configuration::directives()
{
  return directives_;
}

const configuration::Directives& Configuration::directives() const
{
  return directives_;
}

std::vector<SocketConfiguration> Configuration::socket_configurations()
{
  configuration::Servers servers = http_.servers().value();
  assert(servers != configuration::Servers());
  std::vector<SocketConfiguration> socket_configurations;
  // iterate on every server
  for (configuration::Directives::const_iterator it = servers.first; it != servers.second; it++)
  {
    configuration::DirectiveServer* server = static_cast<configuration::DirectiveServer*>(it->second); 
    assert(server != NULL);
    const configuration::DirectivesRange listens = server->query_directive(configuration::Directive::kDirectiveListen);
    assert(listens != configuration::DirectivesRange());
    // iterate on every listen directive in the server
    for (configuration::Directives::const_iterator it2 = listens.first; it2 != listens.second; it2++)
    {
      const configuration::DirectiveListen* listen = static_cast<const configuration::DirectiveListen*>(it2->second);
      const std::vector<configuration::Socket>& sockets = listen->get();
      // iterate on every socket in the listen directive
      for (std::vector<configuration::Socket>::const_iterator it3 = sockets.begin(); it3 != sockets.end(); it3++)
      {
        SocketConfiguration socket_configuration = SocketConfiguration(*it3, server);
        std::vector<SocketConfiguration>::iterator find_result = 
          std::find(socket_configurations.begin(), socket_configurations.end(), socket_configuration);
        // if the socket configuration is not already in the list, add it
        // otherwise, append the server to the existing socket configuration
        if (find_result == socket_configurations.end())
          socket_configurations.push_back(socket_configuration);
        else
          find_result->append(socket_configuration);
      }
    }
  }
  return socket_configurations;
}

size_t Configuration::worker_connections() const
{
  return events_.worker_connections().value();
}
