#pragma once

#include "HTTPMessage.hpp"

// StatusCode is saved outside of the Response class,
// where all information for one process is saved together

// For the Response, version is always HTTP/1.1
// The server can handle requests specifying other versions like HTTP/0.9, 1.0, 2.0,
// however it will remain to be HTTP/1.1 compliant

class Response : public HTTPMessage
{
	public:
		Response();
		Response(Response const &obj);
		~Response();

		Response &operator=(Response const *obj);

	private:
		std::string responseBody_;

		std::string	getResponseBody();

		void	setResponseBody(std::string responseBody);
};