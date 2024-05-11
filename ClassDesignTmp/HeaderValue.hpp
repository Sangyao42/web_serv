#pragma once

#include <string>

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
		virtual const std::string &valueAsString() = 0; // all HeaderValue better has a private string variable to return when this function is called
		virtual const int &valueAsInt() = 0;
};
