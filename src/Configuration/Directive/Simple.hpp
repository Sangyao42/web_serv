#pragma once

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <bitset>

#include "Configuration/Directive.hpp"
#include "Configuration/Socket.hpp"

namespace configuration
{

  //////////////////////////////////////////////////////
  ////////////   All supported directives   ////////////
  //////////////////////////////////////////////////////

  class DirectiveListen;
  class DirectiveServerName;
  class DirectiveAllowMethods; 
  typedef DirectiveSimple<std::string, Directive::kDirectiveRoot> DirectiveRoot;
  typedef DirectiveSimple<std::string, Directive::kDirectiveIndex> DirectiveIndex;
  class DirectiveMimeTypes;
  typedef DirectiveSimple<std::string, Directive::kDirectiveErrorPage> DirectiveErrorPage;
  typedef DirectiveSimple<size_t, Directive::kDirectiveClientMaxBodySize> DirectiveClientMaxBodySize;
  class DirectiveRedirect;
  typedef DirectiveSimple<bool, Directive::kDirectiveAutoindex> DirectiveAutoindex;
  class DirectiveCgi;
  typedef DirectiveSimple<std::string, Directive::kDirectiveAccessLog> DirectiveAccessLog;
  typedef DirectiveSimple<std::string, Directive::kDirectiveErrorLog> DirectiveErrorLog;
  typedef DirectiveSimple<Directive*, Directive::kDirectiveInclude> DirectiveInclude;
  typedef DirectiveSimple<size_t, Directive::kDirectiveWorkerConnections> DirectiveWorkerConnections;

  class DirectiveListen : public Directive
  {
    public:
      DirectiveListen();
      DirectiveListen(const Context& context);
      DirectiveListen(const DirectiveListen& other);
      DirectiveListen& operator=(const DirectiveListen& other);
      virtual ~DirectiveListen();

      virtual bool          is_block() const;
      virtual Type          type() const;

      void                       add(const Socket& socket);
      const std::vector<Socket>& get() const;

    private:
      std::vector<Socket> sockets_;
  };

  class DirectiveServerName : public Directive
  {
    public:
      DirectiveServerName();
      DirectiveServerName(const Context& context);
      DirectiveServerName(const DirectiveServerName& other);
      DirectiveServerName& operator=(const DirectiveServerName& other);
      virtual ~DirectiveServerName();

      virtual bool          is_block() const;
      virtual Type          type() const;

      void                            add(const std::string& server_name);
      const std::vector<std::string>& get() const;

    private:
      std::vector<std::string> server_names_;
  };

  typedef std::bitset<3> Methods;
  enum Method
  {
    kMethodGet    = 1,
    kMethodPost   = 2,
    kMethodDelete = 4
  };

  class DirectiveAllowMethods : public Directive
  {
    public:
      static const int kDefaultMethods = kMethodGet | kMethodPost;
      DirectiveAllowMethods();
      DirectiveAllowMethods(const Context& context);
      DirectiveAllowMethods(const DirectiveAllowMethods& other);
      DirectiveAllowMethods& operator=(const DirectiveAllowMethods& other);
      virtual ~DirectiveAllowMethods();

      virtual bool  is_block() const;
      virtual Type  type() const;

      void          set(int method);
      Methods       get() const;

      bool          is_allowed(Method method) const;

    private:
      Methods  accepted_methods_;
  };

  class DirectiveMimeTypes : public Directive
  {
    public:
      typedef std::string Extension;
      typedef std::string MimeType;
      DirectiveMimeTypes();
      DirectiveMimeTypes(const Context& context);
      DirectiveMimeTypes(const DirectiveMimeTypes& other);
      DirectiveMimeTypes& operator=(const DirectiveMimeTypes& other);
      virtual ~DirectiveMimeTypes();

      virtual bool      is_block() const;
      virtual Type      type() const;

      void              add(const Extension& extension, const MimeType& mime_type);
      const std::map<Extension, MimeType>& get() const;
      Maybe<MimeType>   query(const Extension& extension) const;

    private:
      std::map<Extension, MimeType> mime_types_;
  };

  class DirectiveRedirect : public Directive
  {
    public:
      DirectiveRedirect();
      DirectiveRedirect(const Context& context);
      DirectiveRedirect(const DirectiveRedirect& other);
      DirectiveRedirect& operator=(const DirectiveRedirect& other);
      virtual ~DirectiveRedirect();

      virtual bool        is_block() const;
      virtual Type        type() const;

      void                set(const std::string& path, bool is_permanent);
      const std::string&  get_path() const;
      bool                is_permanent() const;

    private:
      std::string       path_;
      bool              is_permanent_;
  };

  class DirectiveCgi : public Directive
  {
    public:
      DirectiveCgi();
      DirectiveCgi(const Context& context);
      DirectiveCgi(const DirectiveCgi& other);
      DirectiveCgi& operator=(const DirectiveCgi& other);
      virtual ~DirectiveCgi();

      virtual bool  is_block() const;
      virtual Type  type() const;

      void                        set(const std::string& extension, const std::string& cgi_path);
      Maybe<const std::string>   match(std::string extension) const;

    private:
      std::string extension_;
      std::string cgi_path_;
  };
} // namespace configuration
