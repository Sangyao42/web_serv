#include "MimeTypes.hpp"

#include <string>
#include <map>
#include <iostream>

#include "misc/Maybe.hpp"
#include "misc/Nothing.hpp"
#include "Configuration/Directive.hpp"

namespace directive
{
  MimeTypes::MimeTypes()
    : Directive(), mime_types_() {}
  
  MimeTypes::MimeTypes(Nothing)
    : Directive(), mime_types_()
  {
    mime_types_["html"] = "text/html";
    mime_types_["gif"] = "image/gif";
    mime_types_["jpg"] = "image/jpeg";
  }

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

  void MimeTypes::print(int indentation) const
  {
    std::cout << '{' << std::endl;
	for (std::map<Extension, MimeType>::const_iterator it = mime_types_.begin(); it != mime_types_.end(); it++)
	{
      PrintIndentation(indentation);
	  std::cout << '[' << it->first << "]: [" << it->second << ']' << std::endl;
	}
    PrintIndentation(indentation - 2);
    std::cout << '}';
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
