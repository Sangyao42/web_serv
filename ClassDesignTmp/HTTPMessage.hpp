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
		HTTPMessage(HTTPMessage const &obj);
		virtual ~HTTPMessage();

		HTTPMessage &operator=(HTTPMessage const &obj);

	protected:
		// For Request: request, general, entity headers
		// For Response: general, entity headers
		std::map<std::string, HeaderValue *>	headers_;

		// + functions for the map to
		// add headers,
		// find the key and retrieve the value,
		// free memory in the destructor,
		// etc
};
