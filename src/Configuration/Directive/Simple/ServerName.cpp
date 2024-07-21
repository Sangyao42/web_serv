#include "ServerName.hpp"

#include <string>
#include <vector>
#include <iostream>

#include "Configuration/Directive.hpp"

namespace directive
{
  ServerName::ServerName()
    : Directive(), server_names_() {}

  ServerName::ServerName(const Context& context)
    : Directive(context), server_names_() {}
  
  ServerName::ServerName(const ServerName& other)
    : Directive(other), server_names_(other.server_names_) {}
  
  ServerName& ServerName::operator=(const ServerName& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      server_names_ = other.server_names_;
    }
    return *this;
  }

  ServerName::~ServerName() {}

  bool ServerName::is_block() const
  {
    return false;
  }

  Directive::Type ServerName::type() const
  {
    return Directive::kDirectiveServerName;
  }

  void ServerName::print(int) const
  {
    if (server_names_.size() > 0)
	{
      std::vector<std::string>::const_iterator it = server_names_.begin();
	  std::cout << '[' << *it << ']';
      for (; it != server_names_.end(); it++)
	  {
	    std::cout << ", [" << *it << ']';
	  }
	}
  }

  void ServerName::add(const std::string& server_name)
  {
    server_names_.push_back(server_name);
  }

  const std::vector<std::string>& ServerName::get() const
  {
    return server_names_;
  }
} // namespace configuration
