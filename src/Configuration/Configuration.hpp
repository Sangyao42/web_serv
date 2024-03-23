#pragma once

#include "Configuration/Directive.hpp"
#include "Configuration/Directive/Blocks.hpp"

class Configuration
{
  public:
    Configuration();
    Configuration(const Configuration& other);
    Configuration& operator=(const Configuration& other);
    ~Configuration();
  
  private:
    configuration::DirectiveHttp   http_;
    configuration::DirectiveEvents events_;
    configuration::Directives      directives_;
};
