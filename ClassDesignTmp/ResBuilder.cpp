#include "Client.hpp"

void	res_builder::BuildErrorHeaders(struct Client *clt)
{
	// clt->res->addNewPair("Server", something);
	// clt->res->addNewPair("Content-Type", something);
	// clt->res->addNewPair("Connection", something);
	// etc. etc.

	// different headers for different status codes
}

const std::string &res_builder::BuildErrorPage(const struct Client *clt)
{
	std::string response;

	response = "<html>\r\n";
	response += "<head><title>";
	response += StatusCodeAsString(clt->status_code);
	response += "</title></head>\r\n";
	response += "<body>\r\n";
	response += "<center><h1>";
	response += StatusCodeAsString(clt->status_code);
	response += "</h1></center>\r\n";
	response += "<hr><center>Webserv</center>\r\n";
	response += "</body>\r\n";
	response += "</html>\r\n";

	return (response);
}

void	res_builder::GenerateErrorResponse(struct Client *clt)
{
	// build the status line
	std::string response = clt->client_socket->res_buf;
	BuildStatusLine(clt, response);

	// build the headers
	BuildErrorHeaders(clt); // add additional headers according to the error code

	std::string	headers = clt->res->returnMapAsString();
	if (headers.empty()) // stream error occurred
	{
		clt->status_code = k500;
		delete clt->res;
		clt->res = new Response();
		GenerateErrorResponse(clt);
		return ;
	}
	response += headers;

	// build the body

	// first search for error pages in the configuration
	// if not found, generate a default error page

	std::vector<const directive::ErrorPage *>	errorPages = clt->config->query->error_pages;
	std::vector<const directive::ErrorPage *>::const_iterator	it;

	Maybe<std::string> result;
	std::string	pathErrorPage;

	for (it = errorPages.begin(); it != errorPages.end(); ++it)
	{
		result = (*it)->match(clt->status_code);
		if (result.is_ok())
			break ;
	}
	if (it != errorPages.end())
	{
		pathErrorPage = (*it)->file_path();
		if (ReadFileToString(pathErrorPage, response) != kNoError);
		{
			clt->status_code = k500;
			delete clt->res;
			clt->res = new Response();
			GenerateErrorResponse(clt);
			return ;
		}
	}
	response += BuildErrorPage(clt);
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
 