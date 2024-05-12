#include "Client.hpp"

#include <cassert>

void	process::ProcessRequest(struct Client *clt)
{
		if (!clt || !clt->client_socket || !clt->req || !clt->res)
	{
		clt->status_code = k500;
		return (res_builder::GenerateErrorResponse(clt));
	}

	if (clt->status_code != k000)
		return (res_builder::GenerateErrorResponse(clt)); // there is an existing error

	HeaderValue	*Host = clt->req->returnValueAsPointer("Host");

	// query configuration
	clt->config = &ws_database.query(clt->client_socket->socket, \
		(Host ? Host->valueAsString() : ""), \
		clt->req->getRequestTarget().path);

	if (clt->config->is_empty())
	{
		clt->status_code = k500;
		return (res_builder::GenerateErrorResponse(clt));
	}

	assert(clt->config->query && "Location Block in Server block is empty"); //query gives the all the needed info related to server block and location block
	cache::LocationQuery	*location= clt->config->query;
	if (!(location->allowed_methods & (int) clt->req->getMethod()))
	{
		clt->status_code = k405;
		return (res_builder::GenerateErrorResponse(clt));
	}
	if (location->redirect)
		return (res_builder::GenerateRedirectResponse(clt));

	std::string path = process::GetExactPath(location->root, location->match_path, clt->req->getRequestTarget());
	clt->path = path;
	if ((access(path.c_str(), F_OK) != 0) \
	&& (clt->req->getMethod() == kGet || clt->req->getMethod() == kDelete)) // check if the path exists (for get and delete)
	{
		clt->status_code = k404;
		return (res_builder::GenerateErrorResponse(clt));
	}
	else
	{
		if (stat(path.c_str(), &clt->stat_buff) != 0)
		{
			clt->status_code = k500;
			return (res_builder::GenerateErrorResponse(clt));
		}
	}

	HeaderInt *ContentLength = dynamic_cast<HeaderInt *> (clt->req->returnValueAsPointer("Content-Length"));

	if ((ContentLength && ContentLength->content() > location->client_max_body_size) || \
	(clt->req->body_size_chunked_ != -1 && clt->req->body_size_chunked_ > location->client_max_body_size))
	{
		clt->status_code = k413;
		return (res_builder::GenerateErrorResponse(clt));
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
			return (res_builder::GenerateErrorResponse(clt));
	}
}

void	process::ProcessGetRequest(struct Client *clt)
{
	cache::LocationQuery	*location= clt->config->query;
	if (S_ISREG(clt->stat_buff.st_mode))
	{
		if (process::IsCgi(clt->cgi_executable, clt->path, location)) //check file extension and get the cgi path inside IsCgi
			return (process::ProcessGetRequestCgi(clt));
		std::string content_type = process::GetResContentType(clt->path);
		if (!process::IsAccessable(content_type, clt->req->returnValueAsPointer("Accept"), location)) //check Accept header and MIME type && check response entity's content type(based on the extension) and Accept Header
		{
			clt->status_code = k406;
			return (res_builder::GenerateErrorResponse(clt));
		}
		if (access(clt->path.c_str(), R_OK) != 0)
		{
			clt->status_code = k403;
			return (res_builder::GenerateErrorResponse(clt));
		}
		clt->status_code = k200;
		return (res_builder::GenerateSuccessResponse(clt));
	}
	else if (S_ISDIR(clt->stat_buff.st_mode))
	{
		assert(location->indexes.size() && "Indexes is empty");
		std::string index_path = process::GetIndexPath(clt->path, location); // TODO: loop through the location->indexes and check if they exist with F_OK
		// ? update the clt->path to the index file ? which means internal redirect
		clt->path = index_path;
		if (index_path == "")
		{
			if (location->autoindex == true)
				return (res_builder::GenerateAutoindexResponse(clt));
			else
			{
				clt->status_code = k403; // I chose 403 over 404
				return (res_builder::GenerateErrorResponse(clt));
			}
		}
		if (process::IsCgi(clt->cgi_executable, clt->path, location))
			return (ProcessGetRequestCgi(clt));
		std::string content_type = process::GetResContentType(index_path);
		if (!process::IsAccessable(content_type, clt->req->returnValueAsPointer("Accept"), location)) //check Accept header and MIME type && check response entity's content type(based on the extension) and Accept Header
		{
			clt->status_code = k406;
			return (res_builder::GenerateErrorResponse(clt));
		}
		if (access(index_path.c_str(), R_OK) != 0)
		{
			clt->status_code = k403;
			return (res_builder::GenerateErrorResponse(clt));
		}
		clt->status_code = k200;
		return (res_builder::GenerateSuccessResponse(clt));
	}
	else
	{
		clt->status_code = k403;
		return (res_builder::GenerateErrorResponse(clt));
	}
}

void	process::ProcessPostRequest(struct Client *clt)
{
	cache::LocationQuery	*location= clt->config->query;
	if (IsDirFormat(clt->path)) // end with "/"
	{
		clt->status_code = k403;
		return (res_builder::GenerateErrorResponse(clt));
	}

	HeaderValue	*req_content_type = clt->req->returnValueAsPointer("Content-Type");
	if (req_content_type && !IsSupportedMediaType(req_content_type->valueAsString(), location->mime_types)) // checkt content type from request with MIME type
	{
		clt->status_code = k415;
		return (res_builder::GenerateErrorResponse(clt));
	}

	//path is in the clt->path
	if (stat(clt->path.c_str(), &clt->stat_buff) == 0)  //file exists
	{
		if (S_ISDIR(clt->stat_buff.st_mode))
		{
			clt->status_code = k403;
			return (res_builder::GenerateErrorResponse(clt));
		}
		else if (S_ISREG(clt->stat_buff.st_mode))
		{

			if (IsCgi(clt->cgi_executable, clt->path, location))
				return (ProcessPostRequestCgi(clt));
			if(access(clt->path.c_str(), W_OK) != 0)
			{
				clt->status_code = k403;
				return (res_builder::GenerateErrorResponse(clt));
			}
			clt->status_code = k200; // ? what should be sent in the body? could be, eg., <html>Modification Success!</html>
			// TODO: modify the file with the body of the request
			return (res_builder::GenerateSuccessResponse(clt));
		}
		else
		{
			clt->status_code = k403;
			return (res_builder::GenerateErrorResponse(clt));
		}
	}
	else //file does not exist
	{
		if(IsCgi(clt->cgi_executable, clt->path, location))
		{
			clt->status_code = k403;
			return (res_builder::GenerateErrorResponse(clt));
		}
		// upload
		// TODO: check the SEARCH permission for the directory and create the file
		clt->status_code = k201; // + location header to notify where it is saved
		return (res_builder::GenerateSuccessResponse(clt));
	}
}

void	process::ProcessDeleteRequest(struct Client *clt)
{
	cache::LocationQuery	*location= clt->config->query;
	if (IsCgi(clt->cgi_executable, clt->path, location))
	{
		clt->status_code = k405;
		return (res_builder::GenerateErrorResponse(clt));
	}
	if (S_ISREG(clt->stat_buff.st_mode))
	{
		if (access(clt->path.c_str(), W_OK) != 0)
		{
			clt->status_code = k403;
			return (res_builder::GenerateErrorResponse(clt));
		}
		clt->status_code = k204; //or k200 with a success message
		return (res_builder::GenerateSuccessResponse(clt));
	}
	else
	{
		clt->status_code = k403;
		return (res_builder::GenerateErrorResponse(clt));
	}
}

std::string process::GetExactPath(const std::string root, std::string match_path, const struct Uri uri)
{
	(void) match_path;
	std::string exact_path = ".." + root;
	exact_path += uri.path;
	return (exact_path);
}

bool		process::IsCgi(std::string &cgi_executable, std::string path, cache::LocationQuery *location)
{
	assert((path != "") && "clt->path is empty");
	std::string extension = path.substr(path.find_last_of('.') + 1);
	if (extension == path)
		return (false);
	// loop through the location->cgis based on the file extension and check permission
	for (size_t i = 0; i < location->cgis.size(); i++)
	{
		if (location->cgis[i]->match(extension).is_ok())
		{
			cgi_executable = location->cgis[i]->match(extension).value();
			return (true);
		}
	}
	return (false);
}

std::string	process::GetResContentType(std::string path)
{
	assert((path != "") && "clt->path is empty");
	std::string extension = path.substr(path.find_last_of('.') + 1);
	if (extension == path)
		return ("text/plain"); // ? default content type ?
	else
		return (extension);
}

bool	process::IsAccessable(std::string content_type, HeaderValue *accept, cache::LocationQuery *location)
{
	directive::MimeTypes	mime_types;
	// find types in both accept header and location->mime_types
	mime_types = TypeAcceptableAndServable(accept, location->mime_types);
	if (mime_types.get().size() == 0)
		return (false);
	if (mime_types.query(content_type).is_ok())
		return (true);
	else
		return (false);
}

std::string	process::GetIndexPath(std::string path, cache::LocationQuery *location)
{
	std::string index_path;
	for (size_t i = 0; i < location->indexes.size(); i++)
	{
		if (path[path.size() - 1] != '/')
			path += "/";
		index_path = path + location->indexes[i];
		if (access(index_path.c_str(), F_OK) == 0)
			return (index_path);
	}
	return ("");
}

bool		process::IsSupportedMediaType(std::string req_content_type, const directive::MimeTypes* mime_types)
{
	const std::map<Extension, MimeType>	types = mime_types->get();
	std::map<Extension, MimeType>::const_iterator it;
	for (it = types.begin(); it != types.end(); it++)
	{
		if (it->second == req_content_type)
			return (true);
	}
	return (false);
}

bool		process::IsDirFormat(std::string path)
{
	if (path[path.size() - 1] == '/')
		return (true);
	return (false);
}