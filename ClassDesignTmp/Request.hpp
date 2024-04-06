#pragma once

#include "HTTPMessage.hpp"

class Request : public HTTPMessage
{
	public:
		Request();
		Request(const Request &obj);
		~Request();

		Request &operator=(const Request &obj);

		const Method	&getMethod() const;
		const std::string	&getRequestTarget() const;
		const Version	&getVersion() const;
		const std::string	&getRequestBody() const;

		void	setMethod(const Method &method);
		void	setRequestTarget(const std::string &requestTarget);
		void	setVersion(const Version &version);
		void	setRequestBody(const std::string &requestBody);

	private:
		Method	method_;
		std::string	requestTarget_;
		Version	version_;

		std::string requestBody_;
};