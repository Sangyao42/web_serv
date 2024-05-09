#include "Request.hpp"

Request::Request()
: method_(kGet),
	requestTarget_(),
	version_(kStandard),
	requestBody_() {}

Request::Request(const Request &obj)
: method_(obj.method_),
	requestTarget_(obj.requestTarget_),
	version_(obj.version_),
	requestBody_(obj.requestBody_) {}

Request::~Request() {}

Request &Request::operator=(const Request &obj)
{
	method_ = obj.method_;
	requestTarget_ = obj.requestTarget_;
	version_ = obj.version_;
	requestBody_ = obj.requestBody_;
  return *this;
}

const Method	&Request::getMethod() const
{
	return (method_);
}

const std::string	&Request::getRequestTarget() const
{
	return (requestTarget_);
}

const Version	&Request::getVersion() const
{
	return (version_);
}

const std::string	&Request::getRequestBody() const
{
	return (requestBody_);
}

void	Request::setMethod(const Method &method)
{
	method_ = method;
}

void	Request::setRequestTarget(const std::string &requestTarget)
{
	requestTarget_ = requestTarget;
}

void	Request::setVersion(const Version &version)
{
	version_ = version;
}

void	Request::setRequestBody(const std::string &requestBody)
{
	requestBody_ = requestBody;
}
