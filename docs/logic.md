# Pseudo code for the response generation

```c

#include <sys/stat.h>

void	processRequest(struct Client *clt)
{
	if (!clt || !clt->clientSocket || !clt->req || !clt->res)
	{
		clt->statusCode = k500;
		return (generateErrorResponse(clt));
	}

	if (clt->statusCode != k000)
		return (generateErrorResponse(clt)); // there is an existing error

	HeaderValue	*Host = clt->req->returnValueAsPointer("Host");

	// query configuration
	clt->config = ServerQuery(clt->clientSocket->socket, \
		(Host ? host->valueAsString() : "") \
		clt->req->getRequestTarget());

	if (!clt->config)
	{
		clt->statusCode = k500;
		return (generateErrorResponse(clt));
	}

	// query Location
	LocationQuery location;
	LocationBlock* location_block
	location->construct(clt->config, location_block); //TODO: how to query location?
	if (!location)
	{
		clt->statusCode = k404;
		return (generateErrorResponse(clt));
	}

	if (!location->allowed_methods & req->getMethod())
	{
		clt->statusCode = k405;
		return (generateErrorResponse(clt));
	}

	if (location->redirect)
		return (generateRedirectResponse(clt)); // 301 or 307

	std::string path = getExactPath(clt); // TODO: use location->root and location->match_path? and URI

	if ((access(&path, F_OK) != 0) \
	&& (req->getMethod() == kGet || req->getMethod() == kDelete)) // check if the path exists (for get and delete)
	{
		clt->statusCode = k404;
		return (generateErrorResponse(clt));
	}
	else
	{
		if (stat(&path, &clt->statBuff) != 0)
		{
			clt->statusCode = k500;
			return (generateErrorResponse(clt));
		}
	}

	HeaderValue	*Content-Length = clt->req->returnValueAsPointer("Content-Length");

	if (Content-Length && Content-Length.content() > Location->client_max_body_size)
	{
		clt->statusCode = k413;
		return (generateErrorResponse(clt));
	}

	// if (location->cgis) //TODO: does this check the file extension? what if the cgi extension is not supported?
	// {
	// 	processRequestCGI(clt);
	// 	return ;
	// }

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
		return (generateSuccessResponse(clt));
	}

	// the server will search for index files first
	// regardless of the autoindex on/off

	if (match_with_index?)
	{
		clt->path = get_index_path();
		//TODO: check file existance here again for the index file and return ErrorResponse here
		if (is_cgi(index_path))
			return (processGetRequestCGI(clt));
		//TODO: check 406 again
		// process get request
		clt->statusCode = k200;
		return (generateSuccessResponse(clt));
	}

	if (autoindex)
		return (returnAutoindexResponse(clt));
	else
	{
		clt->statusCode = k403; // I chose 403 over 404
		return (generateSuccessResponse(clt));
	}
}

void	processPostRequest(clt)
{
	if (is_directory)
	{
		clt->statusCode = k403;
		return (generateSuccessResponse(clt));
	}

	if (406?) // MIME type checking
	{
		clt->statusCode = k406;
		return (generateErrorResponse(clt));
	} // ? check if this status code is right

	if (is_cgi)
		return (processPostRequestCGI(clt));

	if (is_same_file) // optional
	{
		clt->statusCode = k303;
		return (generateSuccessResponse(clt));
	}

	// upload body as a file or append
	// create all the directories
	if (file_exists)
	{
		// append
		if (415?) // MIME type checking
		{
			clt->statusCode = k415;
			return (generateErrorResponse(clt));
		} // ? not sure about this part
		clt->statusCode = k200; // ? what should be sent in the body?
	}
	else
	{
		// upload
		clt->statusCode = k201; // + location header to notify where it is saved
	}
	return (generateSuccessResponse(clt));
}

void	processDeleteRequest(clt)
{
	if (is_filename? || force_delete?)
	{
		// process delete request
		clt->statusCode = k204; // can be 200 as well
		return (generateSuccessResponse(clt));
	}
	else // is a directory and cannot be deleted
	{
		clt->statusCode = k403; // I chose 403 over 405
		return (generateErrorResponse(clt));
	}
}

// void	processRequestCGI(clt)
// {
// 	if (!is_supported_cgi)

// 	switch(clt->req->getMethod())
// 	{
// 		case kGet:
// 			return (processGetRequestCGI(clt));
// 		case kPost:
// 			return (processPostRequestCGI(clt));;
// 		default:
// 			clt->statusCode = k501;
// 			return (generateErrorResponse(clt));
// 	}
// }

void	processGetRequestCGI(clt)
{
	int	cgi_input[2], cgi_output[2];
	setPipes(cgi_input[2], cgi_output[2], clt->req->getMethods()); //For CGI GET request, only cgi_output[2] is needed
	int pid = fork();
	if (pid < 0)
	{
		close() cgi_output[2] both ends
		clt->statusCode = k500;
		return (generateErrorResponse(clt));
	}
	if (pid == 0)
	{
		//child process
		close(cgi_output[READ]);
		dup2(cgi_output[WRITE], STDOUT);
		CheckCgiFileExtension(); // get which cgi it is
		char* cgi_path = ConstructCgiExcutable(); // get excutable from location->cgis based on the file extension
		char** cgi_argv = ConstructCgiArgv();
		char** cgi_env = ConstructCgiEnv();
		execve(cgi_path, cgi_argv, cgi_env);
		// if execve failed, clean cgi_argv and cgi_env, and close the pipe
		close(cgi_output[WRITE]);
	}
	//parent process
	close(cgi_output[WRITE]);
	dup2(cgi_output[READ],STDIN);
	int read = ReadAll(cgi_output[READ], response_tmp);
	if (read < 0)
	{
		clt->statusCode = k500;
		return (generateErrorResponse(clt));
	}
	ParseResponseTmp(); // modify the clt->response
}

void	processPostRequestCGI(clt)
{

}

```
