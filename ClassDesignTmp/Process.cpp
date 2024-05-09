#include "Client.hpp"

#include <cassert>

void	process::processRequest(struct Client *clt)
{
		if (!clt || !clt->clientSocket || !clt->req || !clt->res)
	{
		clt->statusCode = k500;
		return (resBuilder::generateErrorResponse(clt));
	}

	if (clt->statusCode != k000)
		return (resBuilder::generateErrorResponse(clt)); // there is an existing error

	HeaderValue	*Host = clt->req->returnValueAsPointer("Host");

	// query configuration
	clt->config = &ws_database.query(clt->clientSocket->socket, \
		(Host ? Host->valueAsString() : ""), \
		clt->req->getRequestTarget());

	if (clt->config->is_empty())
	{
		clt->statusCode = k500;
		return (resBuilder::generateErrorResponse(clt));
	}

	assert(clt->config->query && "Location Block in Server block is empty"); //query gives the all the needed info related to server block and location block
	cache::LocationQuery	*location= clt->config->query;
	if (!(location->allowed_methods & (int) clt->req->getMethod()))
	{
		clt->statusCode = k405;
		return (resBuilder::generateErrorResponse(clt));
	}
	if (location->redirect)
		return (resBuilder::generateRedirectResponse(clt));

	std::string path = getExactPath(location->root, location->match_path, clt->req->getRequestTarget());
	if ((access(path.c_str(), F_OK) != 0) \
	&& (clt->req->getMethod() == kGet || clt->req->getMethod() == kDelete)) // check if the path exists (for get and delete)
	{
		clt->statusCode = k404;
		return (resBuilder::generateErrorResponse(clt));
	}
	else
	{
		if (stat(path.c_str(), &clt->statBuff) != 0)
		{
			clt->statusCode = k500;
			return (resBuilder::generateErrorResponse(clt));
		}
	}

	HeaderInt *ContentLength = dynamic_cast<HeaderInt *> (clt->req->returnValueAsPointer("Content-Length"));

	if (ContentLength && ContentLength->content() > location->client_max_body_size)
	{
		clt->statusCode = k413;
		return (resBuilder::generateErrorResponse(clt));
	}

	switch(clt->req->getMethod())
	{
		case kGet:
			return (process::processGetRequest(clt));
		case kPost:
			return (process::processPostRequest(clt));
		case kDelete:
			return (process::processDeleteRequest(clt));
		default:
			clt->statusCode = k501;
			return (resBuilder::generateErrorResponse(clt));
	}
}