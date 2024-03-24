#pragma once

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "Configuration/Directive.hpp"

namespace configuration
{
  class DirectiveListen : public Directive
  {
    public:
      typedef std::pair<std::string, std::string> IpAddress;
      DirectiveListen();
      DirectiveListen(const DirectiveListen& other);
      DirectiveListen& operator=(const DirectiveListen& other);
      virtual ~DirectiveListen();

      virtual bool          is_block() const;
      virtual Type          type() const;

      void                          add(const IpAddress& ip_address);
      const std::vector<IpAddress>& get() const;

    private:
      std::vector<IpAddress> ip_addresses_;
  };

  class DirectiveServerName : public Directive
  {
    public:
      DirectiveServerName();
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

  class DirectiveAllowMethods : public Directive
  {
    public:
      DirectiveAllowMethods();
      DirectiveAllowMethods(const DirectiveAllowMethods& other);
      DirectiveAllowMethods& operator=(const DirectiveAllowMethods& other);
      virtual ~DirectiveAllowMethods();

      virtual bool  is_block() const;
      virtual Type  type() const;

      void          add(Method method);
      const Methods get() const;

    private:
      Methods  accepted_methods_;
  };

  class DirectiveDenyMethods : public Directive
  {
    public:
      DirectiveDenyMethods();
      DirectiveDenyMethods(const DirectiveDenyMethods& other);
      DirectiveDenyMethods& operator=(const DirectiveDenyMethods& other);
      virtual ~DirectiveDenyMethods();

      virtual bool  is_block() const;
      virtual Type  type() const;

      void          add(Method method);
      const Methods get() const;

    private:
      Methods  denied_methods_;
  };

  typedef DirectiveSimple<std::string, Directive::Type::kDirectiveRoot> DirectiveRoot;
  typedef DirectiveSimple<std::string, Directive::Type::kDirectiveIndex> DirectiveIndex;

  class DirectiveMimeTypes : public Directive
  {
    public:
      DirectiveMimeTypes();
      DirectiveMimeTypes(const DirectiveMimeTypes& other);
      DirectiveMimeTypes& operator=(const DirectiveMimeTypes& other);
      virtual ~DirectiveMimeTypes();

      virtual bool          is_block() const;
      virtual Type          type() const;

      bool                add_mime_type(const std::string& type, const std::string& extension);
      const std::string&  get_mime_type(const std::string& extension) const;

    private:
      std::map<std::string, std::string> types_;
  };

  class DirectiveErrorPage : public Directive
  {
    public:
      DirectiveErrorPage();
      DirectiveErrorPage(const DirectiveErrorPage& other);
      DirectiveErrorPage& operator=(const DirectiveErrorPage& other);
      virtual ~DirectiveErrorPage();

      virtual bool        is_block() const;
      virtual Type        type() const;

      void                        add(int error_code, const std::string& path);
      Maybe<const std::string&>   match(int error_code) const;

    private:
      int         error_code_;
      std::string path_;
  };

  typedef DirectiveSimple<size_t, Directive::Type::kDirectiveClientMaxBodySize> DirectiveClientMaxBodySize;

  class DirectiveRedirect : public Directive
  {
    public:
      DirectiveRedirect();
      DirectiveRedirect(const DirectiveRedirect& other);
      DirectiveRedirect& operator=(const DirectiveRedirect& other);
      virtual ~DirectiveRedirect();

      virtual bool        is_block() const;
      virtual Type        type() const;

      void                set(std::string path, bool is_permanent);
      const std::string&  get_path() const;
      bool                is_permanent() const;

    private:
      std::string       path_;
      bool              is_permanent_;
  };

  typedef DirectiveSimple<bool, Directive::Type::kDirectiveAutoindex> DirectiveAutoindex;

  class DirectiveCgi : public Directive
  {
    public:
      DirectiveCgi();
      DirectiveCgi(const DirectiveCgi& other);
      DirectiveCgi& operator=(const DirectiveCgi& other);
      virtual ~DirectiveCgi();

      virtual bool  is_block() const;
      virtual Type  type() const;

      void                        set(const std::string& extension, const std::string& cgi_path);
      Maybe<const std::string&>   match(std::string extension) const;

    private:
      std::string extension_;
      std::string cgi_path_;
  };

  typedef DirectiveSimple<std::string, Directive::Type::kDirectiveAccessLog> DirectiveAccessLog;
  typedef DirectiveSimple<std::string, Directive::Type::kDirectiveErrorLog> DirectiveErrorLog;
  typedef DirectiveSimple<Directive*, Directive::Type::kDirectiveInclude> DirectiveInclude;
  typedef DirectiveSimple<size_t, Directive::Type::kDirectiveWorkerConnections> DirectiveWorkerConnections;
} // namespace configuration
