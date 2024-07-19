#include "Client.hpp"

#include <cstdlib>

void	res_builder::AddLocationHeader(struct Client *clt)
{
	std::string file_path = clt->location_created;
	size_t root_pos = file_path.find(clt->config.query->root) + clt->config.query->root.size() - 1;
	std::string location = file_path.substr(root_pos);
	clt->res.addNewPair("Location", new HeaderString(location));
}

std::string	res_builder::MethodToString(enum directive::Method method)
{
	switch (method)
	{
		case directive::kMethodGet:
			return ("GET");
		case directive::kMethodPost:
			return ("POST");
		case directive::kMethodDelete:
			return ("DELETE");
		default:
			return ("");
	}
}

void	res_builder::AddAllowHeader(struct Client *clt)
{
	StringVector	allows;
	directive::Methods	allowed_methods = clt->config.query->allowed_methods;
	for (int i = 1; i < 8; i *= 2)
		if (allowed_methods & i)
			allows.push_back(MethodToString((enum directive::Method) i));
	clt->res.addNewPair("Allow", new HeaderStringVector(allows));
}

void	res_builder::AddAcceptHeader(struct Client *clt)
{
	StringVector	accepts;
	std::map<directive::MimeTypes::Extension, directive::MimeTypes::MimeType>	mime_types = clt->config.query->mime_types->get();
	std::map<directive::MimeTypes::Extension, directive::MimeTypes::MimeType>::iterator	it;
	for (it = mime_types.begin(); it != mime_types.end(); ++it)
		accepts.push_back(it->second);
	clt->res.addNewPair("Accept", new HeaderStringVector(accepts));
}

void	res_builder::BuildContentHeadersCGI(struct Client *clt)
{
	// add content-length header
	clt->res.addNewPair("Content-Length", new HeaderInt(clt->cgi_content_length));

	// add content-type header
	clt->res.addNewPair("Content-Type", new HeaderString(clt->cgi_content_type));

	// add location header if created
	if (!clt->location_created.empty())
		AddLocationHeader(clt);
}

void	res_builder::BuildContentHeaders(struct Client *clt, std::string extension, std::string path)
{
	// add content-length header
	clt->res.addNewPair("Content-Length", new HeaderInt(clt->res.getResponseBody().size()));

	// add content-type header
  clt->res.addNewPair("Content-Type", new HeaderString(extension));

	// add last-modified header
	struct stat	file_stat;
	if (!path.empty() && stat(path.c_str(), &file_stat) == 0)
	{
		std::string last_modified = GetTimeGMT((time_t) file_stat.st_mtime);
		clt->res.addNewPair("Last-Modified", new HeaderString(last_modified));
	}
}

void	res_builder::ServerError500(struct Client *clt)
{
	clt->status_code = k500;
	clt->res = Response();
	GenerateErrorResponse(clt);
	return ;
}

std::string	res_builder::GetTimeGMT(time_t raw_time)
{
	struct tm	*time_info;
	char		buffer[80];

	time_info = gmtime(&raw_time); // GMT time
	strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", time_info);

	return (std::string(buffer));
}

std::string	res_builder::GetTimeGMT()
{
	time_t	raw_time;
	struct tm	*time_info;
	char		buffer[80];

	time(&raw_time);
	time_info = gmtime(&raw_time); // GMT time
	strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", time_info);

	return (std::string(buffer));
}

void	res_builder::BuildBasicHeaders(Response *res)
{
	res->addNewPair("Server", new HeaderString("Webserv"));
	res->addNewPair("Date", new HeaderString(GetTimeGMT()));
}

void	res_builder::BuildStatusLine(StatusCode status_code, std::string &response)
{
	response = "";
	response += "HTTP/1.1 ";
	response += StatusCodeAsString(status_code);
	response += "\r\n";
}

enum ResponseError	res_builder::ReadFileToBody(const std::string &path, Response *res)
{
	std::ifstream	file(path.c_str(), std::ifstream::in);
	std::stringstream	ss;

	if (!file)
		return (kFileOpenError);

	ss << file.rdbuf();

	if (file.fail())
	 return (kFileStreamError);

	res->setResponseBody(ss.str());
	return (kResponseNoError);
}

std::string	res_builder::StatusCodeAsString(StatusCode code)
{
	switch (code)
	{
		case k200:
			return ("200 OK");
		case k201:
			return ("201 Created");
		case k204:
			return ("204 No Content");
		case k301:
			return ("301 Moved Permanently");
		case k303:
			return ("303 See Other");
		case k304:
			return ("304 Not Modified");
		case k307:
			return ("307 Temporary Redirection");
		case k400:
			return ("400 Bad Request");
		case k403:
			return ("403 Forbidden");
		case k404:
			return ("404 Not Found");
		case k405:
			return ("405 Method Not Allowed");
		case k406:
			return ("406 Not Acceptable");
		case k408:
			return ("408 Request Timeout");
		case k411:
			return ("411 Length Required");
		case k412:
			return ("412 Precondition Failed");
		case k413:
			return ("413 Request Entity Too Large");
		case k414:
			return ("414 URI Too Long");
		case k415:
			return ("415 Unsupported Media Type");
		case k422:
			return ("422 Unprocessable Entity");
		case k500:
			return ("500 Internal Server Error");
		case k501:
			return ("501 Not Implemented");
		case k503:
			return ("503 Service Unavailable");
		default:
		 return (""); // Error
	}
}

