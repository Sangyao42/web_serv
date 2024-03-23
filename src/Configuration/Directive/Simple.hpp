#pragma once

#include <map>
#include <string>

#include "Configuration/Directive.hpp"

namespace configuration
{
  class DirectiveMimeTypes : public Directive
  {
    public:
      DirectiveMimeTypes();
      DirectiveMimeTypes(const DirectiveMimeTypes& other);
      DirectiveMimeTypes& operator=(const DirectiveMimeTypes& other);
      virtual ~DirectiveMimeTypes();

      virtual std::string&  get_name() const;
      virtual bool          is_block() const;
      virtual Type          type() const;

      bool                add_mime_type(const std::string& key, const std::string& value);
      const std::string&  get_mime_type(const std::string& key) const;

    private:
      std::map<std::string, std::string> types_;
  };
} // namespace configuration