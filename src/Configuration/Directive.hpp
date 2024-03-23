#pragma once

#include <map>
#include <string>

#include "misc/Maybe.hpp"

namespace configuration
{
  class Directive;
  class DirectiveServer;
  class DirectiveLocation;
  typedef std::map<std::string, Directive*> Directives;
  typedef std::vector<DirectiveServer*> Servers;
  typedef std::vector<DirectiveLocation*> Locations;

  class Directive
  {
    public:
      struct Context
      {
        int main     : 1;
        int http     : 1;
        int server   : 1;
        int events   : 1;
        int location : 1;
      };

      enum Type
      {
        // Blocks
        kDirectiveHttp,
        kDirectiveServer,
        kDirectiveEvents,
        kDirectiveLocation,
        // for HTTP requests
        kDirectiveListen,
        kDirectiveServerName,
        kDirectiveAllowMethods,
        kDirectiveDenyMethods,
        // for HTTP response generation (serve a file)
        kDirectiveRoot,
        kDirectiveIndex,
        kDirectiveMimeTypes,
        kDirectiveErrorPage,
        // for HTTP request generation (generating content)
        kDirectiveClientMaxBodySize,
        kDirectiveRedirect,
        kDirectiveAutoindex,
        kDirectiveCgi,
        // misc
        kDirectiveAccessLog,
        kDirectiveErrorLog,
        kDirectiveInclude,
        // only in events block
        kDirectiveWorkerConnections,
      };
      virtual ~Directive() {};

      virtual std::string&  get_name() const = 0;
      virtual bool          is_block() const = 0;
      virtual Type          type() const = 0;
  };

  class DirectiveBlock : public Directive
  {
    public:
      DirectiveBlock();
      DirectiveBlock(const DirectiveBlock& other);
      DirectiveBlock& operator=(const DirectiveBlock& other);
      virtual ~DirectiveBlock();

      virtual std::string&  get_name() const = 0;
      virtual bool          is_block() const;
      virtual Type          type() const = 0;

      virtual bool          add_directive(Directive* directive);
      virtual Directive*    get_directive(const std::string& name) const;

    private:
      Directives directives_;
  };
} // namespace configuration
