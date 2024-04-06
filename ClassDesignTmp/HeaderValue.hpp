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
};
