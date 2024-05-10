#include "Client.hpp"

#include <cassert>

void	process::ProcessRequest(struct Client *clt)
{
		if (!clt || !clt->client_socket || !clt->req || !clt->res)
	{
		clt->status_code = k500;
		return (resBuilder::GenerateErrorResponse(clt));
	}

	if (clt->status_code != k000)
		return (resBuilder::GenerateErrorResponse(clt)); // there is an existing error

	HeaderValue	*Host = clt->req->returnValueAsPointer("Host");

	// query configuration
	clt->config = &ws_database.query(clt->client_socket->socket, \
		(Host ? Host->valueAsString() : ""), \
		clt->req->getRequestTarget().path);

	if (clt->config->is_empty())
	{
		clt->status_code = k500;
		return (resBuilder::GenerateErrorResponse(clt));
	}

	assert(clt->config->query && "Location Block in Server block is empty"); //query gives the all the needed info related to server block and location block
	cache::LocationQuery	*location= clt->config->query;
	if (!(location->allowed_methods & (int) clt->req->getMethod()))
	{
		clt->status_code = k405;
		return (resBuilder::GenerateErrorResponse(clt));
	}
	if (location->redirect)
		return (resBuilder::GenerateRedirectResponse(clt));

	std::string path = process::GetExactPath(location->root, location->match_path, clt->req->getRequestTarget());
	clt->path = path;
	if ((access(path.c_str(), F_OK) != 0) \
	&& (clt->req->getMethod() == kGet || clt->req->getMethod() == kDelete)) // check if the path exists (for get and delete)
	{
		clt->status_code = k404;
		return (resBuilder::GenerateErrorResponse(clt));
	}
	else
	{
		if (stat(path.c_str(), &clt->stat_buff) != 0)
		{
			clt->status_code = k500;
			return (resBuilder::GenerateErrorResponse(clt));
		}
	}

	HeaderInt *ContentLength = dynamic_cast<HeaderInt *> (clt->req->returnValueAsPointer("Content-Length"));

	if ((ContentLength && ContentLength->content() > location->client_max_body_size) || \
	(clt->req->body_size_chunked_ != -1 && clt->req->body_size_chunked_ > location->client_max_body_size))
	{
		clt->status_code = k413;
		return (resBuilder::GenerateErrorResponse(clt));
	}

	switch(clt->req->getMethod())
	{
		case kGet:
			return (process::ProcessGetRequest(clt));
		case kPost:
			return (process::ProcessPostRequest(clt));
		case kDelete:
			return (process::ProcessDeleteRequest(clt));
		default:
			clt->status_code = k501;
			return (resBuilder::GenerateErrorResponse(clt));
	}
}

void	process::ProcessGetRequest(struct Client *clt)
{
	cache::LocationQuery	*location= clt->config->query;
	if (S_ISREG(clt->stat_buff.st_mode))
	{
		if (process::IsCgi(clt->path)) //check file extension and get the cgi path inside IsCgi
			return (process::ProcessGetRequestCgi(clt));
		if (!process::IsAccessable(clt->path))
		{
			clt->status_code = k406;
			return (resBuilder::GenerateErrorResponse(clt));
		}
		if (access(clt->path.c_str(), R_OK) != 0)
		{
			clt->status_code = k403;
			return (resBuilder::GenerateErrorResponse(clt));
		}
		clt->status_code = k200;
		return (resBuilder::GenerateSuccessResponse(clt));
	}
	else if (S_ISDIR(clt->stat_buff.st_mode))
	{
		assert(location->indexes.size() && "Indexes is empty");
		std::string index_path = process::GetIndexPath(); // TODO: loop through the location->indexes and check if they exist with F_OK
		// ? update the clt->path to the index file ? which means internal redirect
		clt->path = index_path;
		if (index_path == "")
		{
			if (location->autoindex == true)
				return (resBuilder::GenerateAutoindexResponse(clt));
			else
			{
				clt->status_code = k403; // I chose 403 over 404
				return (resBuilder::GenerateErrorResponse(clt));
			}
		}
		if (process::IsCgi(index_path))
			return (ProcessGetRequestCgi(clt));
		std::string content_type = process::GetContentType(index_path);
		if (!process::IsAccessable(content_type))
		{
			clt->status_code = k406;
			return (resBuilder::GenerateErrorResponse(clt));
		}
		if (access(index_path.c_str(), R_OK) != 0)
		{
			clt->status_code = k403;
			return (resBuilder::GenerateErrorResponse(clt));
		}
		clt->status_code = k200;
		return (resBuilder::GenerateSuccessResponse(clt));
	}
	else
	{
		clt->status_code = k403;
		return (resBuilder::GenerateErrorResponse(clt));
	}
}

void	process::ProcessPostRequest(struct Client *clt)
{
	if (IsDirFormat(clt->path)) // end with "/" or without extension
	{
		clt->status_code = k403;
		return (resBuilder::GenerateErrorResponse(clt));
	}

	HeaderValue	*content_type = clt->req->returnValueAsPointer("Content-Type");
	if (content_type && !IsSupportedMediaType()) // MIME type checking
	{
		clt->status_code = k415;
		return (resBuilder::GenerateErrorResponse(clt));
	}

	//path is in the clt->path
	if (stat(clt->path.c_str(), &clt->stat_buff) == 0)  //file exists
	{
		if (S_ISDIR(clt->stat_buff.st_mode))
		{
			clt->status_code = k403;
			return (resBuilder::GenerateErrorResponse(clt));
		}
		else if (S_ISREG(clt->stat_buff.st_mode))
		{

			if (IsCgi(clt->path))
				return (ProcessPostRequestCgi(clt));
			if(access(clt->path.c_str(), W_OK) != 0)
			{
				clt->status_code = k403;
				return (resBuilder::GenerateErrorResponse(clt));
			}
			clt->status_code = k200; // ? what should be sent in the body? could be, eg., <html>Modification Success!</html>
			// TODO: modify the file with the body of the request
			return (resBuilder::GenerateSuccessResponse(clt));
		}
		else
		{
			clt->status_code = k403;
			return (resBuilder::GenerateErrorResponse(clt));
		}
	}
	else //file does not exist
	{
		if(IsCgi(clt->path))
		{
			clt->status_code = k403;
			return (resBuilder::GenerateErrorResponse(clt));
		}
		// upload
		// TODO: check the SEARCH permission for the directory and create the file
		clt->status_code = k201; // + location header to notify where it is saved
		return (resBuilder::GenerateSuccessResponse(clt));
	}
}

void	process::ProcessDeleteRequest(struct Client *clt)
{
	if (IsCgi(clt->path))
	{
		clt->status_code = k405;
		return (resBuilder::GenerateErrorResponse(clt));
	}
	if (S_ISREG(clt->stat_buff.st_mode))
	{
		if (access(clt->path.c_str(), W_OK) != 0)
		{
			clt->status_code = k403;
			return (resBuilder::GenerateErrorResponse(clt));
		}
		clt->status_code = k204; //or k200 with a success message
		return (resBuilder::GenerateSuccessResponse(clt));
	}
	else
	{
		clt->status_code = k403;
		return (resBuilder::GenerateErrorResponse(clt));
	}
}