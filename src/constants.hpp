#pragma once

#include "Configuration/Directive/Simple/MimeTypes.hpp"
#include "Configuration/Directive/Simple.hpp"
#include "Configuration/Directive/Simple/AllowMethods.hpp"

namespace constants
{
  extern const int                  kDefaultWorkerConnections;

  extern const directive::Methods   kDefaultAllowedMethods;

  extern const size_t               kDefaultClientMaxBodySize;

  extern const std::string          kDefaultRoot;

  extern const directive::Index     kDefaultIndex;

  extern const bool                 kDefaultAutoindex;

  extern const directive::MimeTypes kDefaultMimeTypes;

  extern const std::string          kDefaultAccessLog;

  extern const std::string          kDefaultErrorLog;

} // namespace constants
