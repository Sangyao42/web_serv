#pragma once

#include <vector>

#include "Configuration/Directive/Block/Events.hpp"
#include "Configuration/Directive/Block/Http.hpp"
#include "Configuration/Directive/Block.hpp"

struct Configuration;
extern Configuration ws_configuration;

struct Configuration
{
  directive::HttpBlock    http;
  directive::EventsBlock  events;
  directive::Directives   directives;
  private:
    Configuration& operator=(const Configuration& other);
};
