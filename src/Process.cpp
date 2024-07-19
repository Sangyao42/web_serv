#include "Client.hpp"
#include "Configuration/Directive/Simple/MimeTypes.hpp"

#include <cstdio>

#include <cassert>

void	process::ProcessRequest(struct Client *clt)
{
	assert(clt && clt->client_socket && "client_socket is null");

	if (clt->status_code != k000)
		return (res_builder::GenerateErrorResponse(clt)); // there is an existing error

	HeaderString	*connection = static_cast<HeaderString *> (clt->req.returnValueAsPointer("Connection"));
	if (connection && connection->content() == "close")
		clt->keepAlive = false;

	//check redirect
	if (clt->config.query->redirect)
		return (res_builder::GenerateRedirectResponse(clt));

	switch(clt->req.getMethod())
	{
		case kGet:
			process::ProcessGetRequest(clt);
			break ;
		case kPost:
			process::ProcessPostRequest(clt);
			break ;
		case kDelete:
			process::ProcessDeleteRequest(clt);
			break ;
		default:
			clt->status_code = k501;
			res_builder::GenerateErrorResponse(clt);
	}
	return ;
}

void	process::ProcessGetRequest(struct Client *clt)
{
	cache::LocationQuery	*location= clt->config.query;
	if (S_ISREG(clt->stat_buff.st_mode))
	{
		if (process::IsCgi(clt->cgi_argv, clt->path, location)) //check file extension and get the cgi path inside IsCgi
			return (cgi::ProcessGetRequestCgi(clt));
		// std::string content_type = process::GetReqExtension(clt->path);

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
		if (location->autoindex == true)
		{
			clt->status_code = k200;
			return (res_builder::GenerateAutoindexResponse(clt));
		}
		// autoindex == false, show indexpage or 403
		else
		{
			std::string index_path = process::GetIndexPath(clt->path, location);
			if (index_path == "")
			{
				clt->status_code = k403; // I chose 403 over 404
				return (res_builder::GenerateErrorResponse(clt));
			}
			else
			{
				clt->path = index_path;
				if (process::IsCgi(clt->cgi_argv, clt->path, location))
					return (cgi::ProcessGetRequestCgi(clt));
				if (access(index_path.c_str(), R_OK) != 0)
				{
					clt->status_code = k403;
					return (res_builder::GenerateErrorResponse(clt));
				}
				clt->status_code = k200;
				return (res_builder::GenerateSuccessResponse(clt));
			}
		}
	}
	else
	{
		clt->status_code = k403;
		return (res_builder::GenerateErrorResponse(clt));
	}
}

void	process::ProcessPostRequest(struct Client *clt)
{
	cache::LocationQuery	*location= clt->config.query;

	HeaderString	*req_content_type = static_cast<HeaderString *>(clt->req.returnValueAsPointer("Content-Type"));
	if (req_content_type && !IsSupportedMediaType(req_content_type->content(), location->mime_types)) // checkt content type from request with MIME type
	{
		clt->status_code = k415;
		return (res_builder::GenerateErrorResponse(clt));
	}

	//path is in the clt->path
	if (stat(clt->path.c_str(), &clt->stat_buff) == 0)  //file exists
	{
		if (S_ISDIR(clt->stat_buff.st_mode))
		{
			if (IsDirFormat(clt->path) == false)
				clt->path = clt->path + "/";
			if (file::UploadFile(clt) == false)
			{
				clt->status_code = k500;
				return (res_builder::GenerateErrorResponse(clt));
			}
			else
			{
				clt->status_code = k201; // + location header to notify where it is saved
				return (res_builder::GenerateSuccessResponse(clt));
			}
		}
		else if (S_ISREG(clt->stat_buff.st_mode)) //it is a regular file
		{
			if (IsCgi(clt->cgi_argv, clt->path, location))
				return (cgi::ProcessPostRequestCgi(clt));
			if(access(clt->path.c_str(), W_OK) != 0)
			{
				clt->status_code = k403;
				return (res_builder::GenerateErrorResponse(clt));
			}
			std::string existing_file_extension = GetReqExtension(clt->path);
			if (!existing_file_extension.empty())
			{
				Maybe<directive::MimeTypes::MimeType> mime_type = location->mime_types->query(existing_file_extension);
				if (!mime_type.is_ok())
				{
					clt->status_code = k415;
					return (res_builder::GenerateErrorResponse(clt));
				}
				if (req_content_type && (req_content_type->content() != mime_type.value()))
				{
					clt->status_code = k415;
					return (res_builder::GenerateErrorResponse(clt));
				}
			}
			if (file::ModifyFile(clt) == false)
			{
				clt->status_code = k500;
				return (res_builder::GenerateErrorResponse(clt));
			}
			clt->location_created = clt->path;
			clt->status_code = k200; // ? what should be sent in the body? could be, eg., <html>Modification Success!</html>
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
		if(IsCgi(clt->cgi_argv, clt->path, location))
		{
			clt->status_code = k403;
			return (res_builder::GenerateErrorResponse(clt));
		}
		//upload
		if(file::UploadFile(clt) == false)
		{
			clt->status_code = k500;
			return (res_builder::GenerateErrorResponse(clt));
		}
		clt->status_code = k201; // + location to notify where it is saved
		return (res_builder::GenerateSuccessResponse(clt));
	}
}

void	process::ProcessDeleteRequest(struct Client *clt)
{
	cache::LocationQuery	*location= clt->config.query;
	if (IsCgi(clt->cgi_argv, clt->path, location))
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
		// ? delete the file: do I need to check if remove() fails ?
		if (file::DeleteFile(clt) == false)
		{
			clt->status_code = k500;
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
	// std::string exact_path = "." + root;
	// if (uri.path[0] != '/')
	// 	exact_path += "/";
	std::string exact_path = root;
	exact_path += uri.path;
	return (exact_path);
}

bool		process::IsCgi(std::vector<std::string> &cgi_argv, std::string path, cache::LocationQuery *location)
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
			std::string cgi_executable = location->cgis[i]->match(extension).value();
			cgi_argv.push_back(cgi_executable);
			cgi_argv.push_back(path);
			return (true);
		}
	}
	return (false);
}

std::string	process::GetReqExtension(std::string path)
{
	assert((path != "") && "clt->path is empty");
	std::string extension = path.substr(path.find_last_of('.') + 1);
	if (extension == path)
		return ("");
	else
		return (extension);
}

// bool	process::IsAcceptable(std::string content_type, HeaderValue *accept, cache::LocationQuery *location)
// {
// 	directive::MimeTypes	mime_types;
// 	// find types in both accept header and location->mime_types
// 	mime_types = TypeAcceptableAndServable(accept, location->mime_types);
// 	if (mime_types.get().size() == 0)
// 		return (false);
// 	if (mime_types.query(content_type).is_ok())
// 		return (true);
// 	else
// 		return (false);
// }

std::string	process::GetIndexPath(std::string path, cache::LocationQuery *location)
{
	std::string index_path;
	for (size_t i = 0; i < location->indexes.size(); i++)
	{
		if (path[path.size() - 1] != '/')
			path += "/";
		index_path = path + location->indexes[i]->get();
		std::cerr << "index_path: " << index_path << std::endl;
		if (access(index_path.c_str(), F_OK) == 0)
			return (index_path);
	}
	return ("");
}

bool		process::IsSupportedMediaType(std::string req_content_type, const directive::MimeTypes* mime_types)
{
	const std::map<directive::MimeTypes::Extension, directive::MimeTypes::MimeType>	types = mime_types->get();
	std::map<directive::MimeTypes::Extension, directive::MimeTypes::MimeType>::const_iterator it;
	for (it = types.begin(); it != types.end(); it++)
	{
		if (it->second == req_content_type)
			return (true);
	}
	return (false);
}

bool		process::IsDirFormat(std::string path)
{
	return (path[path.size() - 1] == '/');
}
