#include "Request.hpp"

RequestLine::RequestLine()
: method(kGet),
	request_target(),
	version(kStandard) {}

Request::~Request()
{
  clear_headers();
}

void	Request::clear_headers()
{
  HeaderMap::iterator it;

	for (it = headers.begin(); it != headers.end(); ++it)
		delete (it->second);
	headers.clear();
}

