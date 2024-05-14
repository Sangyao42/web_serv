#include "Client.hpp"

std::string	res_builder::GetTimeGMT()
{
	time_t	raw_time;
	struct tm	*time_info;
	char		buffer[80];

	time(&raw_time);
	time_info = gmtime(&raw_time); // GMT time
	strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", time_info);

	return (std::string(buffer));
}

void	res_builder::BuildBasicHeaders(struct Client *clt)
{
	clt->res->addNewPair("Server", new HeaderString("Webserv"));
	clt->res->addNewPair("Date", new HeaderString(GetTimeGMT()));
}

void	res_builder::BuildStatusLine(const struct Client *clt, std::string &response)
{
	response = "";
	response += "HTTP/1.1 ";
	response += StatusCodeAsString(clt->status_code);
	response += "\r\n";
}

enum ResponseError	res_builder::ReadFileToString(const std::string &path, std::string &response)
{
	std::ifstream	file(path);
	std::stringstream	ss;

	if (!file)
		return (kFileOpenError);

	ss << file.rdbuf();

	if (file.fail())
	 return (kFilestreamError);

	return (kNoError);
}

const std::string	&res_builder::StatusCodeAsString(enum status_code code)
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
		case k303:
			return ("303 See Other");
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
			return ("413 Request Entity Too Large");
		case k414:
			return ("414 URI Too Long");
		case k415:
			return ("415 Unsupported Media Type");
		case k422:
			return ("422 Unprocessable Entity");
		case k500:
			return ("500 Internal Server Error");
		case k503:
			return ("503 Service Unavailable");
		default:
		 return (""); // Error
	}
}
 