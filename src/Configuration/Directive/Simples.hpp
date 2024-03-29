#pragma once

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <bitset>

#include "misc/Maybe.hpp"

#include "./Simple.hpp"
#include "Configuration/Directive.hpp"
#include "Configuration/Socket.hpp"

namespace directive
{

  //////////////////////////////////////////////////////
  ////////////   All supported directives   ////////////
  //////////////////////////////////////////////////////

  class Listen;
  class ServerName;
  class AllowMethods; 
  typedef DirectiveSimple<std::string, Directive::kDirectiveRoot> Root;
  typedef DirectiveSimple<std::string, Directive::kDirectiveIndex> Index;
  class MimeTypes;
  typedef DirectiveSimple<std::string, Directive::kDirectiveErrorPage> ErrorPage;
  typedef DirectiveSimple<size_t, Directive::kDirectiveClientMaxBodySize> ClientMaxBodySize;
  class Redirect;
  typedef DirectiveSimple<bool, Directive::kDirectiveAutoindex> Autoindex;
  class Cgi;
  typedef DirectiveSimple<std::string, Directive::kDirectiveAccessLog> AccessLog;
  typedef DirectiveSimple<std::string, Directive::kDirectiveErrorLog> ErrorLog;
  typedef DirectiveSimple<Directive*, Directive::kDirectiveInclude> Include;
  typedef DirectiveSimple<size_t, Directive::kDirectiveWorkerConnections> WorkerConnections;

  class Listen : public Directive
  {
    public:
      Listen();
      Listen(const Context& context);
      Listen(const Listen& other);
      Listen& operator=(const Listen& other);
      virtual ~Listen();

      virtual bool          is_block() const;
      virtual Type          type() const;

      void                       add(const Socket& socket);
      const std::vector<Socket>& get() const;

    private:
      std::vector<Socket> sockets_;
  };

  class ServerName : public Directive
  {
    public:
      ServerName();
      ServerName(const Context& context);
      ServerName(const ServerName& other);
      ServerName& operator=(const ServerName& other);
      virtual ~ServerName();

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

  class AllowMethods : public Directive
  {
    public:
      static const int kDefaultMethods = kMethodGet | kMethodPost;
      AllowMethods();
      AllowMethods(const Context& context);
      AllowMethods(const AllowMethods& other);
      AllowMethods& operator=(const AllowMethods& other);
      virtual ~AllowMethods();

      virtual bool  is_block() const;
      virtual Type  type() const;

      void          set(int method);
      Methods       get() const;

      bool          is_allowed(Method method) const;

    private:
      Methods  accepted_methods_;
  };

  class MimeTypes : public Directive
  {
    public:
      typedef std::string Extension;
      typedef std::string MimeType;
      MimeTypes();
      MimeTypes(const Context& context);
      MimeTypes(const MimeTypes& other);
      MimeTypes& operator=(const MimeTypes& other);
      virtual ~MimeTypes();

      virtual bool      is_block() const;
      virtual Type      type() const;

      void              add(const Extension& extension, const MimeType& mime_type);
      const std::map<Extension, MimeType>& get() const;
      Maybe<MimeType>   query(const Extension& extension) const;

    private:
      std::map<Extension, MimeType> mime_types_;
  };

  class Redirect : public Directive
  {
    public:
      Redirect();
      Redirect(const Context& context);
      Redirect(const Redirect& other);
      Redirect& operator=(const Redirect& other);
      virtual ~Redirect();

      virtual bool        is_block() const;
      virtual Type        type() const;

      void                set(const std::string& path, bool is_permanent);
      const std::string&  get_path() const;
      bool                is_permanent() const;

    private:
      std::string       path_;
      bool              is_permanent_;
  };

  class Cgi : public Directive
  {
    public:
      Cgi();
      Cgi(const Context& context);
      Cgi(const Cgi& other);
      Cgi& operator=(const Cgi& other);
      virtual ~Cgi();

      virtual bool  is_block() const;
      virtual Type  type() const;

      void                        set(const std::string& extension, const std::string& cgi_path);
      Maybe<const std::string>   match(std::string extension) const;

    private:
      std::string extension_;
      std::string cgi_path_;
  };
} // namespace configuration
