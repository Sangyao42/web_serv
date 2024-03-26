#include "Blocks.hpp"

#include "misc/Maybe.hpp"

namespace configuration
{
  DirectiveHttp::DirectiveHttp()
    : DirectiveBlock(), servers_() {}
  
  DirectiveHttp::DirectiveHttp(const Context& context)
    : DirectiveBlock(context), servers_() {}
  
  DirectiveHttp::DirectiveHttp(const DirectiveHttp& other)
    : DirectiveBlock(other), servers_() {}
  
  DirectiveHttp& DirectiveHttp::operator=(const DirectiveHttp& other)
  {
    if (this != &other)
    {
      DirectiveBlock::operator=(other);
      servers_ = Nothing();
    }
    return *this;
  }

  DirectiveHttp::~DirectiveHttp() {}

  Directive::Type DirectiveHttp::type() const
  {
    return Directive::kDirectiveHttp;
  }

  Maybe<Servers> DirectiveHttp::get_servers()
  {
    if (!servers_.is_ok())
    {
      Servers servers = static_cast<const Directives>(directives_).equal_range(Directive::kDirectiveServer);
      if (servers.first != servers.second)
        servers_ = servers;
    }
    return servers_;
  }
} // namespace configuration
