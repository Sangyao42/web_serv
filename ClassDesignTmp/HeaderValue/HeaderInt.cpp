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
}

const HeaderInt::ValueType	&HeaderInt::type() const
{
	return (type_);
}

const int	&HeaderInt::content() const
{
	return (content_);
}