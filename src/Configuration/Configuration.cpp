#include "Configuration.hpp"

#include <cassert>

#include <vector>

#include "Configuration/Directive/Block/Events.hpp"
#include "Configuration/Directive/Block/Http.hpp"
#include "Configuration/Directive/Block.hpp"
#include "Configuration/Directive/Simple.hpp"

Configuration ws_configuration;

Configuration::Configuration()
  : http_(), events_(), directives_() {}

Configuration::Configuration(const Configuration& other)
  : http_(other.http_), events_(other.events_), directives_(other.directives_) {}

Configuration::~Configuration() {}

directive::HttpBlock& Configuration::http_block()
{
  return http_;
}

const directive::HttpBlock& Configuration::http_block() const
{
  return http_;
}

directive::EventsBlock& Configuration::events_block()
{
  return events_;
}

const directive::EventsBlock& Configuration::events_block() const
{
  return events_;
}

directive::Directives& Configuration::directives()
{
  return directives_;
}

const directive::Directives& Configuration::directives() const
{
  return directives_;
}

size_t Configuration::worker_connections() const
{
  return events_.worker_connections().value();
}
