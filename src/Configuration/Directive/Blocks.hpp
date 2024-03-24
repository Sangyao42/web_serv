#pragma once

#include <string>

#include "misc/Maybe.hpp"

#include "Configuration/Directive.hpp"

namespace configuration
{
  class DirectiveHttp : public DirectiveBlock
  {
    public:
      DirectiveHttp();
      DirectiveHttp(const Context& context);
      DirectiveHttp(const DirectiveHttp& other);
      DirectiveHttp& operator=(const DirectiveHttp& other);
      virtual ~DirectiveHttp();

      virtual Type          type() const;

      Maybe<Servers>        get_servers();

    private:
      Maybe<Servers> servers_;
  };

  class DirectiveServer : public DirectiveBlock
  {
    public:
      DirectiveServer();
      DirectiveServer(const Context& context);
      DirectiveServer(const DirectiveServer& other);
      DirectiveServer& operator=(const DirectiveServer& other);
      virtual ~DirectiveServer();

      virtual Type          type() const;

      Maybe<Locations>      get_locations();

    private:
      Maybe<Locations> locations_;
  };

  class DirectiveEvents : public DirectiveBlock
  {
    public:
      DirectiveEvents();
      DirectiveEvents(const Context& context);
      DirectiveEvents(const DirectiveEvents& other);
      DirectiveEvents& operator=(const DirectiveEvents& other);
      virtual ~DirectiveEvents();

      virtual Type          type() const;
  };

  class DirectiveLocation : public DirectiveBlock
  {
    public:
      DirectiveLocation();
      DirectiveLocation(const Context& context);
      DirectiveLocation(const DirectiveLocation& other);
      DirectiveLocation& operator=(const DirectiveLocation& other);
      virtual ~DirectiveLocation();

      virtual Type          type() const;

      Maybe<Locations>      get_locations();

    private:
      Maybe<Locations> locations_;
  };
} // namespace configuration
