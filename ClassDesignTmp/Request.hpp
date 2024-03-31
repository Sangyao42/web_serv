#pragma once

#include "HTTPMessage.hpp"

class Request : public HTTPMessage
{
	public:
		Request();
		Request(Request const &obj);
		~Request();

		Request &operator=(Request const &obj);

	private:
		Method	method_;
		std::string	requestTarget_;
		std::string	version_;

		std::string requestBody_;

		Method	getMethod();
		std::string	getRequestTarget();
		std::string	getVersion();
		std::string	getRequestBody();

		void	setMethod(Method method);
		void	setRequestTarget(std::string requestTarget);
		void	setVersion(std::string version);
		void	setRequestBody(std::string requestBody);
};