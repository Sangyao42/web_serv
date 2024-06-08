#include "Response.hpp"

Response::Response()
: responseBody_() {}

Response::Response(const Response &obj)
: responseBody_(obj.responseBody_) {}

Response::~Response() {}

Response &Response::operator=(const Response &obj)
{
	responseBody_ = obj.responseBody_;
}

const std::string	&Response::getResponseBody() const
{
	return (responseBody_);
}

void	Response::setResponseBody(const std::string &responseBody)
{
	responseBody_ = responseBody;
}
