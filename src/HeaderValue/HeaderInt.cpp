#include "HeaderInt.hpp"
#include <cstdio>

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
	// std::snprintf(buff, 12, "%d", content_);
	int int_len = 0;
	int tmp = content_;
	if (tmp == 0)
		int_len = 1;
	while (tmp)
	{
		tmp = tmp / 10;
		int_len++;
	}
	long new_n = static_cast<long>(content_);
	buff[int_len] = '\0';
	while (int_len)
	{
		buff[--int_len] = new_n % 10 + '0';
		new_n = new_n / 10;
	}
	return std::string(buff);
}

