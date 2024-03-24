#include "Simple.hpp"

#include <map>
#include <string>

#include "misc/Maybe.hpp"
#include "misc/Nothing.hpp"

namespace configuration
{
  DirectiveMimeTypes::DirectiveMimeTypes()
    : Directive(), mime_types_() {}

  DirectiveMimeTypes::DirectiveMimeTypes(const Context& context)
    : Directive(context), mime_types_() {}

  DirectiveMimeTypes::DirectiveMimeTypes(const DirectiveMimeTypes& other)
    : Directive(other), mime_types_(other.mime_types_) {}

  DirectiveMimeTypes& DirectiveMimeTypes::operator=(const DirectiveMimeTypes& other)
  {
    if (this != &other)
    {
      Directive::operator=(other);
      mime_types_ = other.mime_types_;
    }
    return *this;
  }

  DirectiveMimeTypes::~DirectiveMimeTypes() {}

  bool DirectiveMimeTypes::is_block() const
  {
    return false;
  }

  Directive::Type DirectiveMimeTypes::type() const
  {
    return Directive::kDirectiveMimeTypes;
  }

  void DirectiveMimeTypes::add(const std::string& extension, const std::string& mime_type)
  {
    mime_types_[extension] = mime_type;
  }

  Maybe<const std::string> DirectiveMimeTypes::get(const std::string& extension) const
  {
    std::map<std::string, std::string>::const_iterator it = mime_types_.find(extension);
    if (it != mime_types_.end())
      return it->second;
    return Nothing();
  }
} // namespace configuration
