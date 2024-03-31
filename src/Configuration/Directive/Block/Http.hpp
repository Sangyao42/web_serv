#pragma once

#include "Configuration/Directive/Block.hpp"

namespace directive
{
  /**
   * @brief Servers points to a range of Directives that are of type
   * DirectiveServer.
  */
  typedef DirectivesRange Servers;

  class HttpBlock : public DirectiveBlock
  {
    public:
      HttpBlock();
      HttpBlock(const Context& context);
      HttpBlock(const HttpBlock& other);
      HttpBlock& operator=(const HttpBlock& other);
      virtual ~HttpBlock();

      virtual Type  type() const;

      Servers       servers() const;
  };
} // namespace configuration
