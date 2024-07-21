#include "Request.hpp"

RequestLine::RequestLine()
: method(kGet),
	request_target(),
	version(kStandard) {}

Request::Request()
: HTTPMessage(),
	method_(kGet),
	request_target_(),
	version_(kStandard),
	requestBody_() {}

Request::Request(const Request &obj)
:	HTTPMessage(obj),
	method_(obj.method_),
	request_target_(obj.request_target_),
	version_(obj.version_),
	requestBody_(obj.requestBody_) {}

Request::~Request() {}

Request &Request::operator=(const Request &obj)
{
	method_ = obj.method_;
	request_target_ = obj.request_target_;
	version_ = obj.version_;
	requestBody_ = obj.requestBody_;
	return (*this);
}

const Method	&Request::getMethod() const
{
	return (method_);
}

const struct Uri	&Request::getRequestTarget() const
{
	return (request_target_);
}

const Version	&Request::getVersion() const
{
	return (version_);
}

const std::string	&Request::getRequestBody() const
{
	return (requestBody_);
}

void  Request::setRequestLine(const RequestLine &request_line)
{
  method_ = request_line.method;
  request_target_ = request_line.request_target;
  version_ = request_line.version;
}

void	Request::setMethod(const Method &method)
{
	method_ = method;
}

void	Request::setRequestTarget(const struct Uri &requestTarget)
{
	request_target_ = requestTarget;
}

void	Request::setVersion(const Version &version)
{
	version_ = version;
}

void	Request::setRequestBody(const std::string &requestBody)
{
	requestBody_ = requestBody;
}

void	Request::reset()
{
	method_ = kGet;
	request_target_ = Uri();
	version_ = kStandard;
	requestBody_.clear();
}
