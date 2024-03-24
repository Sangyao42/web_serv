#include "Simple.hpp"

#include <string>
#include <vector>

namespace configuration
{
  DirectiveServerName::DirectiveServerName()
    : Directive(), server_names_() {}

  DirectiveServerName::DirectiveServerName(const Context& context)
    : Directive(context), server_names_() {}
  
  DirectiveServerName::DirectiveServerName(const DirectiveServerName& other)
    : Directive(other), server_names_(other.server_names_) {}
  
  DirectiveServerName& DirectiveServerName::operator=(const DirectiveServerName& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      server_names_ = other.server_names_;
    }
    return *this;
  }

  DirectiveServerName::~DirectiveServerName() {}

  bool DirectiveServerName::is_block() const
  {
    return false;
  }

  Directive::Type DirectiveServerName::type() const
  {
    return Directive::kDirectiveServerName;
  }

  void DirectiveServerName::add(const std::string& server_name)
  {
    server_names_.push_back(server_name);
  }

  const std::vector<std::string>& DirectiveServerName::get() const
  {
    return server_names_;
  }
} // namespace configuration
