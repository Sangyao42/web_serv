#include "Client.hpp"
#include <cassert>

void	res_builder::BuildRedirectResponseBody(struct Client *clt)
{
	std::string	body;

	body = "<!DOCTYPE html>\r\n";
	body += "<html>\r\n";

	if (clt->status_code == k301)
	{
		body += "<head><title>Resource Moved Permanently</title></head>\r\n";
		body += "<body>\r\n";
		body += "<h1>Resource Moved Permanently</h1>\r\n";
		body += "</body>\r\n";
		body += "</html>\r\n";
	}
	else
	{
		body += "<head><title>Resource Moved Temporarily</title></head>\r\n";
		body += "<body>\r\n";
		body += "<h1>Resource Moved Temporarily</h1>\r\n";
		body += "</body>\r\n";
		body += "</html>\r\n";
	}

	clt->res.setResponseBody(body);
}

void	res_builder::GenerateRedirectResponse(struct Client *clt)
{
	const directive::Return *redirect = clt->config.query->redirect;
	clt->status_code = (StatusCode) redirect->status_code();

	assert((clt->status_code == k301 || clt->status_code == k307) &&  "Invalid status code for redirect");

	// build the status line
	std::string &response = clt->client_socket->res_buf;
	BuildStatusLine(clt->status_code, response);

	// build basic headers
	BuildBasicHeaders(&clt->res);

	std::string location = clt->config.query->redirect->get_path();
	clt->res.addNewPair("Location", new HeaderString(location));

	// build the body and content headers
	BuildRedirectResponseBody(clt);
	BuildContentHeaders(clt, "html", "");

	// add headers to the response
	std::string	headers = clt->res.returnMapAsString();
	if (headers.empty()) // stream error occurred
	{
		ServerError500(clt);
		return ;
	}
	response += headers;

	// add body to response
	response += clt->res.getResponseBody();
}

