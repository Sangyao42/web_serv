#include "Client.hpp"

void	resBuilder::helper::buildErrorHeaders(struct Client *clt)
{
	// clt->res->addNewPair("Server", something);
	// clt->res->addNewPair("Content-Type", something);
	// clt->res->addNewPair("Connection", something);
	// etc. etc.

	// different headers for different status codes
}

const std::string &resBuilder::helper::buildErrorPage(const struct Client *clt)
{
	std::string response;

	response = "<html>\r\n";
	response += "<head><title>";
	response += statusCodeAsString(clt->statusCode);
	response += "</title></head>\r\n";
	response += "<body>\r\n";
	response += "<center><h1>";
	response += statusCodeAsString(clt->statusCode);
	response += "</h1></center>\r\n";
	response += "<hr><center>Webserv</center>\r\n";
	response += "</body>\r\n";
	response += "</html>\r\n";

	return (response);
}

void	resBuilder::generateErrorResponse(struct Client *clt)
{
	// build the status line
	std::string response = clt->clientSocket->res_buf;
	buildStatusLine(clt, response);

	// build the headers
	buildErrorHeaders(clt); // add headers according to the error code
	response += clt->res->returnMapAsString();

	// build the body

	// first search for error pages in the configuration
	// if not found, generate a default error page

	std::vector<const directive::ErrorPage *>	errorPages = clt.config->query->error_pages;
	std::vector<const directive::ErrorPage *>::const_iterator	it;

	Maybe<std::string> result;
	std::string	pathErrorPage;

	for (it = errorPages.begin(); it != errorPages.end(); ++it)
	{
		result = (*it)->match(clt->statusCode);
		if (result.is_ok())
			break ;
	}
	if (it != errorPages.end())
	{
		pathErrorPage = (*it)->file_path;
		response += readFileToString(pathErrorPage);
	}	
	else
		response += buildErrorPage(clt);
}

void	resBuilder::utils::buildStatusLine(const struct Client *clt, std::string &response)
{
	response = "";
	response += "HTTP/1.1 ";
	response += statusCodeAsString(clt->statusCode);
	response += "\r\n";
}

const std::string &resBuilder::utils::readFileToString(const std::string &path)
{
	std::ifstream	file(path);
	std::stringstream	ss;

	if (!file)
		// throw error
	if (file.fail())
		// throw error

	ss << file.rdbuf();

	return (ss.str());
}

const std::string	&resBuilder::utils::statusCodeAsString(enum StatusCode code)
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
		default:
		 return (""); // Error
	}	
}
