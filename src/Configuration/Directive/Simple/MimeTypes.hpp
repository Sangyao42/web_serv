#pragma once

#include <string>
#include <map>

#include "misc/Maybe.hpp"
#include "Configuration/Directive.hpp"

namespace directive
{
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

      virtual bool                  is_block() const;
      virtual Type                  type() const;

      void                          add(const Extension& extension, const MimeType& mime_type);
      const std::map<Extension, MimeType>& get() const;
      Maybe<MimeType>               query(const Extension& extension) const;

    private:
      std::map<Extension, MimeType> mime_types_;
  };
} // namespace configuration
