#pragma once

#include <map>
#include "Protocol.hpp"
#include "HeaderValue.hpp"

/*
	An abstract class having information and functions
	shared by Request and Response classes */

class HTTPMessage
{
	public:
		HTTPMessage();
		HTTPMessage(HTTPMessage const &obj);
		virtual ~HTTPMessage();

		HTTPMessage &operator=(HTTPMessage const &obj);

	protected:
		Method	method_;

		std::string	requestURL_;

		Version version_;

		std::map<std::string, HeaderValue *> requestHeaders_;

		std::string body_;
};
