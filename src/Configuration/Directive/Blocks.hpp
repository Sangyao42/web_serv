#pragma once

#include <string>

#include "Configuration/Directive.hpp"

namespace configuration
{
  class DirectiveServer;
  class DirectiveHttp : public DirectiveBlock
  {
    public:
      DirectiveHttp();
      DirectiveHttp(const DirectiveHttp& other);
      DirectiveHttp& operator=(const DirectiveHttp& other);
      virtual ~DirectiveHttp();

      virtual Type          type() const;

      bool                  add_directive(Directive* directive);
      const Servers&        get_servers() const;

    private:
      Servers servers_;
  };

  class DirectiveLocation;
  class DirectiveServer : public DirectiveBlock
  {
    public:
      DirectiveServer();
      DirectiveServer(const DirectiveServer& other);
      DirectiveServer& operator=(const DirectiveServer& other);
      virtual ~DirectiveServer();

      virtual Type          type() const;

      bool                  add_directive(Directive* directive);
      const Locations&      get_locations() const;

    private:
      Locations locations_;
  };

  class DirectiveEvents : public DirectiveBlock
  {
    public:
      DirectiveEvents();
      DirectiveEvents(const DirectiveEvents& other);
      DirectiveEvents& operator=(const DirectiveEvents& other);
      virtual ~DirectiveEvents();

      virtual Type          type() const;
  };

  class DirectiveLocation : public DirectiveBlock
  {
    public:
      DirectiveLocation();
      DirectiveLocation(const DirectiveLocation& other);
      DirectiveLocation& operator=(const DirectiveLocation& other);
      virtual ~DirectiveLocation();

      virtual Type          type() const;

      bool                  add_directive(Directive* directive);
      const Locations&      get_locations() const;

    private:
      Locations locations_;
  };
} // namespace configuration