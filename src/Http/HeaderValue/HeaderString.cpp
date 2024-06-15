#include "HeaderString.hpp"

HeaderString::HeaderString()
: content_() {}

HeaderString::HeaderString(const std::string &content)
: content_(content) {}

HeaderString::HeaderString(const HeaderString &obj)
: content_(obj.content_) {}

HeaderString::~HeaderString() {}

HeaderString &HeaderString::operator=(const HeaderString &obj)
{
	content_ = obj.content_;
  return *this;
}

HeaderString::ValueType	HeaderString::type() const
{
	return kString;
}

std::string	&HeaderString::content()
{
	return (content_);
}

std::string HeaderString::valueAsString()
{
	return (content_);
}

