#pragma once

#include <string>

#include "misc/Maybe.hpp"

#include "./Block.hpp"
#include "Configuration/Directive.hpp"

namespace directive
{
  class HttpBlock : public DirectiveBlock
  {
    public:
      HttpBlock();
      HttpBlock(const Context& context);
      HttpBlock(const HttpBlock& other);
      HttpBlock& operator=(const HttpBlock& other);
      virtual ~HttpBlock();

      virtual Type          type() const;

      Maybe<Servers>        servers();

    private:
      Maybe<Servers> servers_;
  };

  class ServerBlock : public DirectiveBlock
  {
    public:
      ServerBlock();
      ServerBlock(const Context& context);
      ServerBlock(const ServerBlock& other);
      ServerBlock& operator=(const ServerBlock& other);
      virtual ~ServerBlock();

      virtual Type          type() const;

      Maybe<Locations>      locations();

    private:
      Maybe<Locations> locations_;
  };

  class EventsBlock : public DirectiveBlock
  {
    public:
      EventsBlock();
      EventsBlock(const Context& context);
      EventsBlock(const EventsBlock& other);
      EventsBlock& operator=(const EventsBlock& other);
      virtual ~EventsBlock();

      virtual Type          type() const;

      Maybe<size_t>         worker_connections() const;
  };

  class LocationBlock : public DirectiveBlock
  {
    public:
      LocationBlock();
      LocationBlock(const Context& context);
      LocationBlock(const LocationBlock& other);
      LocationBlock& operator=(const LocationBlock& other);
      virtual ~LocationBlock();

      virtual Type          type() const;

      Maybe<Locations>      locations();

    private:
      Maybe<Locations> locations_;
  };
} // namespace configuration
