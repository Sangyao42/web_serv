#pragma once

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "misc/Maybe.hpp"

namespace configuration
{
  class Directive
  {
    public:
      struct Context
      {
        int index;

        Context();
        Context(int index);

        bool  operator==(const Context& context) const;
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
        kDirectiveWorkerConnections
      };
      Directive();
      Directive(const Context& context);
      Directive(const Directive& other);
      Directive& operator=(const Directive& other);
      virtual ~Directive();

      virtual bool          is_block() const = 0;
      virtual Type          type() const = 0;
      const Context&        context() const;
      void                  set_context(const Context& context);
      int                   index() const;

    protected:
      Context context_;
  };

  class DirectiveServer;
  class DirectiveLocation;

  /**
   * @brief Directives is a map of directive names to their corresponding
   * Directive objects. It is possible to have multiple entry for the same
   * directive name, in which case the order of insertion is preserved. Therefore
   * multimap is used.
  */
  typedef std::multimap<Directive::Type, Directive*> Directives;

  typedef std::pair<Directives::const_iterator, Directives::const_iterator> DirectivesRange;

  /**
   * @brief Servers points to a range of Directives that are of type
   * DirectiveServer.
  */
  typedef DirectivesRange Servers;

  /**
   * @brief Locations points to a range of Directives that are of type
   * DirectiveLocation.
  */
  typedef DirectivesRange Locations;

  class DirectiveBlock : public Directive
  {
    public:
      DirectiveBlock();
      DirectiveBlock(const Context& context);
      DirectiveBlock(const DirectiveBlock& other);
      DirectiveBlock& operator=(const DirectiveBlock& other);
      virtual ~DirectiveBlock();

      virtual bool            is_block() const;
      virtual Type            type() const = 0;

      virtual void            add_directive(Directive* directive);
      virtual DirectivesRange get_directive(Type type) const;

    protected:
      Directives directives_;
  };

  template <typename T, Directive::Type TypeEnum>
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

  template <typename T, Directive::Type TypeEnum>
  DirectiveSimple<T, TypeEnum>::DirectiveSimple()
    : Directive(), value_() {}
  
  template <typename T, Directive::Type TypeEnum>
  DirectiveSimple<T, TypeEnum>::DirectiveSimple(const Context& context)
    : Directive(context), value_() {}

  template <typename T, Directive::Type TypeEnum>
  DirectiveSimple<T, TypeEnum>::DirectiveSimple(const DirectiveSimple& other)
    : Directive(other), value_(other.value_) {}
  
  template <typename T, Directive::Type TypeEnum>
  DirectiveSimple<T, TypeEnum>& DirectiveSimple<T, TypeEnum>::operator=(const DirectiveSimple& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      value_ = other.value_;
    }
    return *this;
  }

  template <typename T, Directive::Type TypeEnum>
  DirectiveSimple<T, TypeEnum>::~DirectiveSimple() {}

  template <typename T, Directive::Type TypeEnum>
  bool DirectiveSimple<T, TypeEnum>::is_block() const
  {
    return false;
  }

  template <typename T, Directive::Type TypeEnum>
  Directive::Type DirectiveSimple<T, TypeEnum>::type() const
  {
    return TypeEnum;
  }

  template <typename T, Directive::Type TypeEnum>
  void DirectiveSimple<T, TypeEnum>::set(const T& value)
  {
    value_ = value;
  }

  template <typename T, Directive::Type TypeEnum>
  const T& DirectiveSimple<T, TypeEnum>::get() const
  {
    return value_;
  }
} // namespace configuration
