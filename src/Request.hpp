#pragma once

#include "HTTPMessage.hpp"

struct RequestLine
{
  Method      method;
  struct Uri  request_target;
  Version     version;

  RequestLine();
};

class Request : public HTTPMessage
{
	public:
		Request();
		Request(const Request &obj);
		~Request();

		Request &operator=(const Request &obj);

		const Method	&getMethod() const;
		const struct Uri	&getRequestTarget() const;
		const Version	&getVersion() const;
		const std::string	&getRequestBody() const;

    void  setRequestLine(const RequestLine &request_line);
		void	setMethod(const Method &method);
		void	setRequestTarget(const struct Uri &requestTarget);
		void	setVersion(const Version &version);
		void	setRequestBody(const std::string &requestBody);

	public:
    Method	    method_;
    struct Uri	request_target_;
    Version	    version_;
		std::string requestBody_;
};

