#pragma once

#include <vector>

#include "Configuration/Directive/Block/Events.hpp"
#include "Configuration/Directive/Block/Http.hpp"
#include "Configuration/Directive/Block.hpp"

class Configuration;
extern Configuration ws_configuration;

class Configuration
{
  public:
    Configuration();
    Configuration(const Configuration& other);
    ~Configuration();

    directive::HttpBlock&         http_block();
    const directive::HttpBlock&   http_block() const;
    directive::EventsBlock&       events_block();
    const directive::EventsBlock& events_block() const;
    directive::Directives&        directives();
    const directive::Directives&  directives() const;

    size_t                        worker_connections() const;

  private:
    directive::HttpBlock          http_;
    directive::EventsBlock        events_;
    directive::Directives         directives_;

    Configuration& operator=(const Configuration& other);
};
