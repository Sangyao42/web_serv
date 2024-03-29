#include "Http.hpp"

#include "misc/Maybe.hpp"
#include "Configuration/Directive.hpp"
#include "Configuration/Directive/Block.hpp"
#include "Configuration/Directive/Block/Server.hpp"

namespace directive
{
  HttpBlock::HttpBlock()
    : DirectiveBlock(), servers_() {}
  
  HttpBlock::HttpBlock(const Context& context)
    : DirectiveBlock(context), servers_() {}
  
  HttpBlock::HttpBlock(const HttpBlock& other)
    : DirectiveBlock(other), servers_() {}
  
  HttpBlock& HttpBlock::operator=(const HttpBlock& other)
  {
    if (this != &other)
    {
      DirectiveBlock::operator=(other);
      servers_ = Nothing();
    }
    return *this;
  }

  HttpBlock::~HttpBlock() {}

  Directive::Type HttpBlock::type() const
  {
    return Directive::kDirectiveHttp;
  }

  Maybe<Servers> HttpBlock::servers()
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
