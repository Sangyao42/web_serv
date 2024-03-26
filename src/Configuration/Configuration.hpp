#pragma once

#include <vector>

#include "Configuration/SocketConfiguration.hpp"
#include "Configuration/Directive.hpp"
#include "Configuration/Directive/Blocks.hpp"
#include "Configuration/Directive/Simple.hpp"

class Configuration;
extern Configuration ws_configuration;

class Configuration
{
  public:
    Configuration();
    Configuration(const Configuration& other);
    Configuration& operator=(const Configuration& other);
    ~Configuration();

    configuration::DirectiveHttp&         http_block();
    const configuration::DirectiveHttp&   http_block() const;
    configuration::DirectiveEvents&       events_block();
    const configuration::DirectiveEvents& events_block() const;
    configuration::Directives&            directives();
    const configuration::Directives&      directives() const;

    /**
     * @return configuration for each listening socket
    */
    std::vector<SocketConfiguration>      socket_configurations();
    size_t                                worker_connections() const;

  private:
    configuration::DirectiveHttp   http_;
    configuration::DirectiveEvents events_;
    configuration::Directives      directives_;
};
