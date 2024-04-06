#include "Http.hpp"

#include "Configuration/Directive.hpp"
#include "Configuration/Directive/Block.hpp"
#include "Configuration/Directive/Block/Server.hpp"

namespace directive
{
  HttpBlock::HttpBlock()
    : DirectiveBlock() {}
  
  HttpBlock::HttpBlock(const Context& context)
    : DirectiveBlock(context) {}
  
  HttpBlock::HttpBlock(const HttpBlock& other)
    : DirectiveBlock(other) {}
  
  HttpBlock& HttpBlock::operator=(const HttpBlock& other)
  {
    if (this != &other)
    {
      DirectiveBlock::operator=(other);
    }
    return *this;
  }

  HttpBlock::~HttpBlock() {}

  Directive::Type HttpBlock::type() const
  {
    return Directive::kDirectiveHttp;
  }

  Servers HttpBlock::servers() const
  {
    return directives_.equal_range(Directive::kDirectiveServer);
  }
} // namespace configuration
