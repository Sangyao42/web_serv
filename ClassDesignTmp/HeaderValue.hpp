#pragma once

#include <string>
#include <sstream>

/*
	An interface class which serves as a base class
	for classes storing different data types */

class HeaderValue
{
	public:
		virtual ~HeaderValue();

		enum ValueType
		{
			kString,
			kInt
		};

		virtual	const ValueType	&type() const = 0;
		virtual HeaderValue	*clone() const = 0;
		virtual void toStringStream(std::ostringstream &oss) = 0;
};

std::ostringstream	&operator<<(std::ostringstream &oss, HeaderValue *obj)
{
	obj->toStringStream(oss);
	return (oss);
}
