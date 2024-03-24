#pragma once

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "misc/Maybe.hpp"

namespace configuration
{
  class Directive;
  class DirectiveServer;
  class DirectiveLocation;

  /**
   * @brief Directives is a map of directive names to their corresponding
   * Directive objects. It is possible to have multiple entry for the same
   * directive name, in which case the order of insertion is preserved. Therefore
   * multimap is used.
  */
  typedef std::multimap<std::string, Directive*> Directives;

  /**
   * @brief Servers points to a range of Directives that are of type
   * DirectiveServer.
  */
  typedef std::pair<Directives::iterator, Directives::iterator> Servers;

  /**
   * @brief Locations points to a range of Directives that are of type
   * DirectiveLocation.
  */
  typedef std::pair<Directives::iterator, Directives::iterator> Locations;

  struct Methods
  {
    int get_bit : 1;
    int post_bit : 1;
    int delete_bit : 1;
  };

  enum Method
  {
    kMethodGet,
    kMethodPost,
    kMethodDelete,
  };

  class Directive
  {
    public:
      struct Context
      {
        int index;

        Context();
        Context(int index);
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
      Directive();
      Directive(const Context& context);
      Directive(const Directive& other);
      Directive& operator=(const Directive& other);
      virtual ~Directive() {};

      virtual bool          is_block() const = 0;
      virtual Type          type() const = 0;
      const Context&        context() const;
      int                   index() const;

    protected:
      Context context_;
  };

  class DirectiveBlock : public Directive
  {
    public:
      DirectiveBlock();
      DirectiveBlock(const Context& context);
      DirectiveBlock(const DirectiveBlock& other);
      DirectiveBlock& operator=(const DirectiveBlock& other);
      virtual ~DirectiveBlock();

      virtual bool          is_block() const;
      virtual Type          type() const = 0;

      virtual bool          add_directive(Directive* directive);
      virtual Directive*    get_directive(Type type) const;

    protected:
      Directives directives_;
  };

  template <typename T, Directive::Type type>
  class DirectiveSimple : public Directive
  {
    public:
      DirectiveSimple();
      DirectiveSimple(const Context& context);
      DirectiveSimple(const DirectiveSimple& other);
      DirectiveSimple& operator=(const DirectiveSimple& other);
      virtual ~DirectiveSimple();

      virtual bool        is_block() const;
      virtual Type        type() const;

      void                set(const T& value);
      const T&            get() const;

    private:
      T value_;
  };

  template <typename T, Directive::Type type>
  DirectiveSimple<T, type>::DirectiveSimple()
    : Directive(), value_() {}
  
  template <typename T, Directive::Type type>
  DirectiveSimple<T, type>::DirectiveSimple(const Context& context)
    : Directive(context), value_() {}

  template <typename T, Directive::Type type>
  DirectiveSimple<T, type>::DirectiveSimple(const DirectiveSimple& other)
    : Directive(other), value_(other.value_) {}
  
  template <typename T, Directive::Type type>
  DirectiveSimple<T, type>& DirectiveSimple<T, type>::operator=(const DirectiveSimple& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      value_ = other.value_;
    }
    return *this;
  }

  template <typename T, Directive::Type type>
  DirectiveSimple<T, type>::~DirectiveSimple() {}

  template <typename T, Directive::Type type>
  bool DirectiveSimple<T, type>::is_block() const
  {
    return false;
  }

  template <typename T, Directive::Type type>
  Directive::Type DirectiveSimple<T, type>::type() const
  {
    return type;
  }

  template <typename T, Directive::Type type>
  void DirectiveSimple<T, type>::set(const T& value)
  {
    value_ = value;
  }

  template <typename T, Directive::Type type>
  const T& DirectiveSimple<T, type>::get() const
  {
    return value_;
  }
} // namespace configuration
