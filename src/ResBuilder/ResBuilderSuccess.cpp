#include "Client.hpp"
#include <cassert>

void	res_builder::BuildPostResponseBody(struct Client *clt)
{
	std::string body;

	std::string path = clt->location_created;

	if (path.compare(0, 5,"./www") == 0)
		path = "http://localhost:8080" + path.substr(5);

	assert((clt->status_code == k201 || clt->status_code == k200) && "Invalid status code");

	body = "<!DOCTYPE html>\r\n";
	body += "<html>\r\n";

	if (clt->status_code == k201)
	{
		body += "<head><title>Resource Created</title></head>\r\n";
		body += "<body>\r\n";
		body += "<h1>Resource Created</h1>\r\n";
		body += "<p>Your resource has been created successfully.</p>\r\n";
		body += "<p>Location";
		body += path;
		body += "</p>\r\n";
		body += "</body>\r\n";
		body += "</html>\r\n";
	}
	else
	{
		body += "<head><title>Resource Modified</title></head>\r\n";
		body += "<body>\r\n";
		body += "<h1>Resource Modified</h1>\r\n";
		body += "<p>Your resource has been modified successfully.</p>\r\n";
		body += "<p>Location: ";
		body += path;
		body += "</p>\r\n";
		body += "</body>\r\n";
		body += "</html>\r\n";
	}

	clt->res.setResponseBody(body);
}

void	res_builder::GenerateSuccessResponse(struct Client *clt)
{
	// build the status line
	std::string &response = clt->client_socket->res_buf;
	BuildStatusLine(clt->status_code, response);

	// build basic headers
	BuildBasicHeaders(&clt->res);

	// build the body and content headers
	if (!clt->cgi_argv.empty())
	{
		BuildContentHeadersCGI(clt);
	}
	else
	{
		if (clt->req.getMethod() == kGet) // it is not a cgi request
		{
			if (ReadFileToBody(clt->path, &clt->res) != kResponseNoError)
			{
				ServerError500(clt);
				return ;
			}
			std::string extension = process::GetReqExtension(clt->path);
			if (extension == "" || extension == "txt")
				extension = "text/plain";
			else if (extension == "html" || extension == "htm")
				extension = "text/html";
			else if (extension == "css")
				extension = "text/css";
			else if (extension == "js")
				extension = "text/javascript";
			else if (extension == "json")
				extension = "application/json";
			else if (extension == "png")
				extension = "image/png";
			else if (extension == "jpg" || extension == "jpeg")
				extension = "image/jpeg";
			else
				extension = "application/octet-stream";
			BuildContentHeaders(clt, extension, clt->path);
		}
		else if (clt->req.getMethod() == kPost) // it is not a cgi request
		{
			AddAllowHeader(clt);
			AddLocationHeader(clt);
			BuildPostResponseBody(clt);
			BuildContentHeaders(clt, "text/html", "");
		}
		else
			assert(clt->req.getMethod() == kDelete && "Invalid method");
	}

	// add headers to the response
	std::string	headers = clt->res.returnMapAsString();
	if (headers.empty()) // stream error occurred
	{
		ServerError500(clt);
		return ;
	}
	response += headers;

	// add body to response
	if (!clt->res.getResponseBody().empty())
		response += clt->res.getResponseBody();
}

