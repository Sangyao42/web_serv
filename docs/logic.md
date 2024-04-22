# Pseudo code for the response generation

```c

void	processRequest(struct Client *clt)
{
	if (!clt->clientSocket || !clt->req || !clt->res)
		// something went wrong unexpectedly, struct empty

	if (statusCode != k000)
		return (generateErrorResponse(clt)); // there is an existing error

	HeaderValue	*Host = clt->req->returnValueAsPointer("Host")

	// query configuration
	clt->config = query(clt->clientSocket->socket, \
		Host ? Host->valueAsString() : "" \
		clt->req->getRequestTarget());

	if (!clt->config)
		// something went wrong while querying

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

// where/where else do we check
// 403 Forbidden and 406 Not Acceptable?
void	processGetRequest(clt)
{
	if (!config->query->allowed_methods & directive::kMethodGet)
	{
		clt->statusCode = k405;
		return (generateErrorResponse(clt));
	}

	if (is_filename?)
	{
		if (is_cgi?)
			return (processGetRequestCGI(clt));

		if (404?)
		{
			clt->statusCode = k404;
			return (generateErrorResponse(clt));
		}

		// process get request
		clt->statusCode = k200;
		return (generateFileResponse(clt));
	}

	// the server will search for index files first
	// regardless of the autoindex
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
		// 403 or 404 depending on the detailed configuration
		// i guess we can simply choose one of two
		clt->statusCode = k403 or k404;
		return (generateFileResponse(clt));
	}
}

void	processPostRequest(clt)
{
	if (!config->query->allowed_methods & directive::kMethodPost)
	{
		clt->statusCode = k405;
		return (generateErrorResponse(clt));
	}

	...
}

void	processDeleteRequest(clt)
{
	if (!config->query->allowed_methods & directive::kMethodDelete)
	{
		clt->statusCode = k405;
		return (generateErrorResponse(clt));
	}

	if (is_filename?)
	{
		if (404?)
		{
			clt->statusCode = k404;
			return (generateErrorResponse(clt));
		}

		if (403?) // access denied?
		{
			clt->statusCode = k403;
			return (generateErrorResponse(clt));
		}

		// process delete request
		clt->statusCode = k204; // can be k200 as well
		return (generateFileResponse(clt));
	}
	else // is a directory
	{
		clt->statusCode = k403; // I chose 403 over 405
		return (generateErrorResponse(clt));
	}
}

```