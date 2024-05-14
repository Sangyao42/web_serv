#include "Client.hpp"

void	res_builder::BuildErrorHeaders(struct Client *clt)
{
	// different headers for different status codes
	switch (clt->status_code)
	{
		case k405:
			// clt->config->query->allowed_methods
			// clt->res->addNewPair("Allow", new HeaderSomething());
			break ;
		case k406:
			// supported media types
			// clt->res->addNewPair("Accept", new HeaderSomething());
			break ;
		case k408:
			// clt->res->addNewPair("Connection", new HeaderString("close"));
			break ;
		case k415:
			// supported media types
			// clt->res->addNewPair("Accept", new HeaderSomething());
			break ;
	}
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
	BuildBasicHeaders(clt); // add basic headers
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
