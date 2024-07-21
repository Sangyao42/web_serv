#pragma once

#include "HTTPMessage.hpp"

class Response : public HTTPMessage
{
	public:
		Response();
		Response(const Response &obj);
		~Response();

		Response &operator=(const Response &obj);

		const std::string	&getResponseBody() const;

		void	setResponseBody(const std::string &responseBody);
		void	reset();

	private:
		std::string responseBody_;
};

