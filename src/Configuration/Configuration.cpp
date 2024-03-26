#include "Configuration.hpp"

#include <cassert>
#include <vector>

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

size_t Configuration::worker_connections() const
{
  return events_.worker_connections().value();
}
