#include "HeaderInt.hpp"

HeaderInt::HeaderInt()
: type_(kInt), content_(-1) {}

HeaderInt::HeaderInt(const int &content)
: type_(kInt), content_(content) {}

HeaderInt::HeaderInt(const HeaderInt &obj)
: type_(obj.type_), content_(obj.content_) {}

HeaderInt::~HeaderInt() {}

HeaderInt &HeaderInt::operator=(const HeaderInt &obj)
{
	type_ = obj.type_;
	content_ = obj.content_;
  return *this;
}

const HeaderInt::ValueType	&HeaderInt::type() const
{
	return (type_);
}

const int	&HeaderInt::content() const
{
	return (content_);
}

HeaderInt	*HeaderInt::clone() const
{
	return (new HeaderInt(*this));
}

std::string HeaderInt::to_string() const
{
  char  buff[12];
	std::snprintf(buff, 12, "%d", content_);
  return std::string(buff);
}

