#pragma once

#include <map>
#include "Protocol.hpp"
#include "HeaderValue.hpp"

/*
	An abstract class having data types and functions
	shared by Request and Response classes */

class HTTPMessage
{
	public:
		HTTPMessage();
		HTTPMessage(const HTTPMessage &obj);
		virtual ~HTTPMessage();

		HTTPMessage &operator=(const HTTPMessage &obj);

	protected:
		std::map<std::string, HeaderValue *>	headers_;

		// + functions for the map to
		// add headers,
		// find the key and retrieve the value,
		// free memory in the destructor,
		// etc
};
