#pragma once

#include "Configuration/Directive.hpp"
#include <iostream>

namespace directive
{
  template <typename T, Directive::Type TypeEnum>
  class DirectiveSimple : public Directive
  {
    public:
      DirectiveSimple();
      DirectiveSimple(const T& value);
      explicit DirectiveSimple(const Context& context);
      DirectiveSimple(const DirectiveSimple& other);
      DirectiveSimple& operator=(const DirectiveSimple& other);
      virtual ~DirectiveSimple();

      virtual bool  is_block() const;
      virtual Type  type() const;
	  virtual void  print(int) const;

      void          set(const T& value);
      const T&      get() const;

    private:
      T             value_;
  };

  //////////////////////////////////////////////////////
  ////////////   All supported directives   ////////////
  //////////////////////////////////////////////////////

  typedef DirectiveSimple<std::string, Directive::kDirectiveRoot> Root;
  typedef DirectiveSimple<std::string, Directive::kDirectiveIndex> Index;
  typedef DirectiveSimple<size_t, Directive::kDirectiveClientMaxBodySize> ClientMaxBodySize;
  typedef DirectiveSimple<bool, Directive::kDirectiveAutoindex> Autoindex;
  typedef DirectiveSimple<std::string, Directive::kDirectiveAccessLog> AccessLog;
  typedef DirectiveSimple<std::string, Directive::kDirectiveErrorLog> ErrorLog;
  typedef DirectiveSimple<size_t, Directive::kDirectiveWorkerConnections> WorkerConnections;

  //////////////////////////////////////////////////////
  ////////////   Template implementation   /////////////
  //////////////////////////////////////////////////////

  template <typename T, Directive::Type TypeEnum>
  DirectiveSimple<T, TypeEnum>::DirectiveSimple()
    : Directive(), value_() {}
  
  template <typename T, Directive::Type TypeEnum>
  DirectiveSimple<T, TypeEnum>::DirectiveSimple(const T& value)
    : Directive(), value_(value) {}
  
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
  void DirectiveSimple<T, TypeEnum>::print(int) const
  {
	std::cout << "[" << value_ << "]";
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
} // namespace directive
