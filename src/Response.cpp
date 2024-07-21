#include "Response.hpp"

Response::Response()
: 	HTTPMessage(),
	responseBody_() {}

Response::Response(const Response &obj)
: 	HTTPMessage(obj),
	responseBody_(obj.responseBody_) {}

Response::~Response() {}

Response &Response::operator=(const Response &obj)
{
	responseBody_ = obj.responseBody_;
	return (*this);
}

const std::string	&Response::getResponseBody() const
{
	return (responseBody_);
}

void	Response::setResponseBody(const std::string &responseBody)
{
	responseBody_ = responseBody;
}

void Response::reset()
{
	cleanHeaderMap();
	responseBody_.clear();
}
