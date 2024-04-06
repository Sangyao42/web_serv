#include "constants.hpp"

#include "Uri/Authority.hpp"
#include "Configuration/Directive/Simple/MimeTypes.hpp"
#include "Configuration/Directive/Simple.hpp"
#include "Configuration/Directive/Simple/AllowMethods.hpp"

namespace constants
{
  const int kDefaultWorkerConnections = 1024;

  const uri::Authority  kDefaultAuthority;

  const directive::Methods  kDefaultAllowedMethods = directive::kMethodGet | directive::kMethodDelete;

  const size_t  kDefaultClientMaxBodySize = (1 << 20); // 1 MB

  const std::string  kDefaultRoot = "html";

  const directive::Index  kDefaultIndex("index.html");

  const bool  kDefaultAutoindex = false;

  const directive::MimeTypes  kDefaultMimeTypes = Nothing();

  const std::string  kDefaultAccessLog = "logs/access.log";

  const std::string  kDefaultErrorLog = "logs/error.log";

} // namespace constants
