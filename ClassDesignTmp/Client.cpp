#include "Client.hpp"

const std::string	&statusCodeAsString(enum StatusCode code)
{
	switch (code)
	{
		case k200:
			return ("200 OK");
		case k201:
			return ("201 Created");
		case k204:
			return ("204 No Content");
		case k301:
			return ("301 Moved Permanently");
		case k304:
			return ("304 Not Modified");
		case k307:
			return ("307 Temporary Redirection");
		case k400:
			return ("400 Bad Request");
		case k403:
			return ("403 Forbidden");
		case k404:
			return ("404 Not Found");
		case k405:
			return ("405 Method Not Allowed");
		case k406:
			return ("406 Not Acceptable");
		case k408:
			return ("408 Request Timeout");
		case k412:
			return ("412 Precondition Failed");
		case k413:
			return ("Request Entity Too Large");
		case k414:
			return ("414 URI Too Long");
		case k415:
			return ("Unsupported Media Type");
		case k422:
			return ("Unprocessable Entity");
		case k500:
			return ("500 Internal Server Error");
		case k503:
			return ("503 Service Unavailable");
		// ...
		default:
		 return (""); // Error
	}	
}
