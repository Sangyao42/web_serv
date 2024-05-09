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
		if (!file_is_R_OK)
		{
			clt->statusCode = k403;
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
	//checking if the path has a directory format, ? or check it with stats() ?
	if (is_directoryPath)
	{
		clt->statusCode = k403;
		return (generateErrorResponse(clt));
	}

	HeaderValue	*Content-Type = clt->req->returnValueAsPointer("Content-Type");
	if (Content-Type && !is_mime_type) // MIME type checking
	{
		clt->statusCode = k415;
		return (generateErrorResponse(clt));
	}

	if (is_same_file) // optional
	{
		clt->statusCode = k303;
		return (generateSuccessResponse(clt));
	}

	// upload body as a file or modified version of the existing file
	// create all the directories
	if (!file_exists)
	{
		if(is_cgi)
		{
			clt->statusCode = k403;
			return (generateSuccessResponse(clt));
		}
		// upload
		// ? check the SEARCH permission for the directory ? //
		clt->statusCode = k201; // + location header to notify where it is saved
	}
	else
	{
		if(is_cgi)
			return (processPostRequestCGI(clt));
		// append is done by cgi, but the body will be the modified version of the existing file, see PUT Method
		if (!access(file, W_OK))
		{
			clt->statusCode = k403;
			return (generateSuccessResponse(clt));
		}
		clt->statusCode = k200; // ? what should be sent in the body? could be, eg., <html>Modification Success!</html>
	}
	return (generateSuccessResponse(clt));
}

void	processDeleteRequest(clt)
{
	if (is_cgi)
	{
		clt->statusCode = k403;
		return (generateErrorResponse(clt));
	}
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

void	processGetRequestCGI(clt)
{
	int	cgi_input[2], cgi_output[2];
	SetPipes(cgi_input[2], cgi_output[2], clt->req->getMethods()); //For CGI GET request, only cgi_output[2] is needed
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
		char* cgi_path = ConstructCgiExcutable(); // get excutable from location->cgis based on the file extension and check permission
		if (!executable_file_is_X_OK || !cgi_is_R_OK)
		{
			close(cgi_output[WRITE]);
			clt->statusCode = k500;
			return (generateErrorResponse(clt));
		}
		char** cgi_argv = ConstructCgiArgv();
		char** cgi_env = ConstructCgiEnv();
		execve(cgi_path, cgi_argv, cgi_env);
		// TODO: if execve failed, clean cgi_argv and cgi_env, and close the pipe
		close(cgi_output[WRITE]);
	}
	//parent process
	std::string response_tmp;
	close(cgi_output[WRITE]);
	int wstats;
	waitpid(pid, &wstats, 0);
	if (WIFEXITED(wtsats) && WEXITSTATUS(wstats) == 0)
	{
		int read = ReadAll(cgi_output[READ], response_tmp);
		close(cgi_output[READ]);
		if (read < 0)
		{
			clt->statusCode = k500;
			return (generateErrorResponse(clt));
		}
		ParseResponseTmp(); // modify the clt->response by the received message from cgi
		//generate responses based on the status code read from the cgi output
		if (clt->statusCode && clt->statusCode >= k400)
			return (generateErrorResponse(clt));
		if (content-type != clt->req->returnValueAsPointer("Accept"))
		{
			clt->statusCode = k406;
			return (generateErrorResponse(clt));
		}
		else
		{
			clt->statusCode = k200;
			return (generateSuccessResponse(clt))
		}
	}
	else
	{
		clt->statusCode = k500;
		return (generateErrorResponse(clt));
	}
}

void	processPostRequestCGI(clt)
{
	int	cgi_input[2], cgi_output[2];
	SetPipes(cgi_input[2], cgi_output[2], clt->req->getMethods()); //For CGI POST request, cgi_input[2] and cgi_output[2] are both needed
	int pid = fork();
	if (pid < 0)
	{
		close() cgi_input[2] both ends
		close() cgi_output[2] both ends
		clt->statusCode = k500;
		return (generateErrorResponse(clt));
	}
	if (pid == 0)
	{
		//child process
		close(cgi_output[READ]);
		dup2(cgi_output[WRITE], STDOUT);
		close(cgi_input[WRITE]);
		dup2(cgi_input[READ], STDIN);
		CheckCgiFileExtension(); // get which cgi it is
		char* cgi_path = ConstructCgiExcutable(); // get excutable from location->cgis based on the file extension and check permission
		if (!executable_file_is_X_OK || !cgi_is_R_OK)
		{
			clt->statusCode = k500;
			return (generateErrorResponse(clt));
		}
		char** cgi_argv = ConstructCgiArgv();
		char** cgi_env = ConstructCgiEnv();
		execve(cgi_path, cgi_argv, cgi_env);
		// TODO: if execve failed, clean cgi_argv and cgi_env, and close the pipe
		close(cgi_output[WRITE]);
		close(cgi_input[READ]);
	}
	//parent process
	//write to child process
	close(cgi_input[READ]);
	int write_byte = write(cgi_input[WRITE], clt->req->body->content(), clt->req->body->content().size());
	if (write_byte < 0)
	{
		close(cgi_input[WRITE]);
		close(cgi_output[READ]);
		close(cgi_output[WRITE]);
		clt->statusCode = k500;
		return (generateErrorResponse(clt));
	}
	close(cgi_input[WRITE]);

	//read from child process
	std::string response_tmp;
	close(cgi_output[WRITE]);
	int wstats;
	waitpid(pid, &wstats, 0);
	if (WIFEXITED(wtsats) && WEXITSTATUS(wstats) == 0)
	{
		int read = ReadAll(cgi_output[READ], response_tmp);
		close(cgi_output[READ])
		if (read < 0)
		{
			clt->statusCode = k500;
			return (generateErrorResponse(clt));
		}
		ParseResponseTmp(); // modify the clt->response by the received message from cgi
		//generate responses based on the status code read from the cgi output
		if (clt->statusCode && clt->statusCode >= k400)
			return (generateErrorResponse(clt));
		if (content-type != clt->req->returnValueAsPointer("Accept"))
		{
			clt->statusCode = k406;
			return (generateErrorResponse(clt));
		}
		else
		{
			clt->statusCode = k200;
			return (generateSuccessResponse(clt))
		}
	}
	else
	{
		clt->statusCode = k500;
		return (generateErrorResponse(clt));
	}

}

```
