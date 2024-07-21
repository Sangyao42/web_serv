#pragma once

#include <string>
#include <vector>

#include "Configuration/Directive.hpp"

namespace directive
{
  class ServerName : public Directive
  {
    public:
      ServerName();
      ServerName(const Context& context);
      ServerName(const ServerName& other);
      ServerName& operator=(const ServerName& other);
      virtual ~ServerName();

      virtual bool                    is_block() const;
      virtual Type                    type() const;
	  virtual void					  print(int) const;

      void                            add(const std::string& server_name);
      const std::vector<std::string>& get() const;

    private:
      std::vector<std::string>        server_names_;
  };
} // namespace configuration
