#include "Simples.hpp"

#include <map>
#include <string>

#include "misc/Maybe.hpp"
#include "misc/Nothing.hpp"

namespace directive
{
  MimeTypes::MimeTypes()
    : Directive(), mime_types_() {}

  MimeTypes::MimeTypes(const Context& context)
    : Directive(context), mime_types_() {}

  MimeTypes::MimeTypes(const MimeTypes& other)
    : Directive(other), mime_types_(other.mime_types_) {}

  MimeTypes& MimeTypes::operator=(const MimeTypes& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      mime_types_ = other.mime_types_;
    }
    return *this;
  }

  MimeTypes::~MimeTypes() {}

  bool MimeTypes::is_block() const
  {
    return false;
  }

  Directive::Type MimeTypes::type() const
  {
    return Directive::kDirectiveMimeTypes;
  }

  void MimeTypes::add(const MimeTypes::Extension& extension, const MimeTypes::MimeType& mime_type)
  {
    mime_types_[extension] = mime_type;
  }

  const std::map<MimeTypes::Extension, MimeTypes::MimeType>& MimeTypes::get() const
  {
    return mime_types_;
  }

  Maybe<MimeTypes::MimeType> MimeTypes::query(const MimeTypes::Extension& extension) const
  {
    std::map<std::string, std::string>::const_iterator it = mime_types_.find(extension);
    if (it != mime_types_.end())
      return it->second;
    return Nothing();
  }
} // namespace configuration
