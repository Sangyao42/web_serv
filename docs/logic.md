# Pseudo code for the response generation

```c

void	processRequest(struct Client *clt)
{
	if (!clt || !clt->clientSocket || !clt->req || !clt->res)
	{
		clt->statusCode = k500;
		return (generateErrorResponse(clt));
	}

	if (clt->statusCode != k000)
		return (generateErrorResponse(clt)); // there is an existing error

	HeaderValue	*host = clt->req->returnValueAsPointer("Host")

	// query configuration
	clt->config = query(clt->clientSocket->socket, \
		(host ? host->valueAsString() : "") \
		clt->req->getRequestTarget());

	if (!clt->config)
	{
		clt->statusCode = k500;
		return (generateErrorResponse(clt));
	}

	if (!config->query->allowed_methods & req->getMethod())
	{
		clt->statusCode = k405;
		return (generateErrorResponse(clt));
	}

	if (404?) // check if the path is valid (for get and delete)
	{
		clt->statusCode = k404;
		return (generateErrorResponse(clt));
	}

	if (clt->req->getRequestBody().size() > client_max_body_size)
	{
		clt->statusCode = k413;
		return (generateErrorResponse(clt));
	}

	if (redirect)
		return (generateRedirectResponse(clt)); // 301 or 307

	switch(clt->req->getMethod())
	{
		case kGet:
			return (processGetRequest(clt));
		case kPost:
			return (processPostRequest(clt));
		case kDelete:
			return (processDeleteRequest(clt));
		default:
			clt->statusCode = k501;
			return (generateErrorResponse(clt));
	}
}

void	processGetRequest(clt)
{
	if (is_filename?)
	{
		if (is_cgi?)
			return (processGetRequestCGI(clt));

		if (406?) // MIME type checking
		{
			clt->statusCode = k406;
			return (generateErrorResponse(clt));
		}
	
		// process get request
		// save the content of the file into res using res->setResponseBody()
		clt->statusCode = k200;
		return (generateFileResponse(clt));
	}

	// the server will search for index files first
	// regardless of the autoindex on/off

	if (match_with_index?)
	{
		// process get request
		clt->statusCode = k200;
		return (generateFileResponse(clt));
	}

	if (autoindex)
		return (returnAutoindexResponse(clt));
	else
	{
		clt->statusCode = k403; // I chose 403 over 404
		return (generateFileResponse(clt));
	}
}

void	processPostRequest(clt)
{
	if ()
	...
}

void	processDeleteRequest(clt)
{
	if (is_filename? || force_delete?)
	{
		// process delete request
		clt->statusCode = k204; // can be 200 as well
		return (generateFileResponse(clt)); // 204 does not have a body
	}
	else // is a directory and cannot be deleted
	{
		if (force_delete?)
			// process delete request

		clt->statusCode = k403; // I chose 403 over 405
		return (generateErrorResponse(clt));
	}
}

```