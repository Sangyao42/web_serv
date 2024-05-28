#include "Client.hpp"
#include <cassert>

std::string res_builder::BuildAutoindexHTML(DSet files, std::string path)
{
	DSetIt it;

	std::string html = "<!DOCTYPE html>\r\n";
	html += "<html>\r\n";
	html += "<head><style>";
	html += "ul { display: inline-block; }";
	html += "</style><title>Index of";
	html += path;
	html += "</title></head>\r\n";
	html += "<body>\r\n";
	html += "<h1>Index of ";
	html += path;
	html += "</h1>\r\n";
	html += "<ul>";

	for (it = ++(files.begin()); it != files.end(); ++it)
	{
		html += "<li><a href=\"";
		if (it->first == DT_DIR)
			html += it->second;
		html += "\">";
		html += it->second;
		html += "</a></li>\r\n";
	}

	html += "</ul>\r\n";
	html += "</body>\r\n";
	html += "</html>\r\n";

	return (html);
}

void	res_builder::GenerateAutoindexResponse(struct Client *clt)
{
	// build the status line
	std::string response = clt->client_socket->res_buf;
	BuildStatusLine(clt->status_code, response);

	// build basic headers
	BuildBasicHeaders(clt->res);

	// build autoindex body
	struct dirent *dirent;
	std::string path = clt->path;
	DIR	*dir_stream = opendir(path.c_str());

	if (dir_stream == NULL)
	{
		assert(errno == ENOENT || errno == ENOTDIR || errno == EACCES && "Unexpected error opening directory");
		ServerError500(clt);
		return ;
	}

	DSet files;

	while ((dirent = readdir(dir_stream)) != NULL)
	{
		files.insert(std::make_pair(dirent->d_type, dirent->d_name));
	}

	if (errno != 0)
	{
		ServerError500(clt);
		closedir(dir_stream);
		return ;
	}

	std::string html = BuildAutoindexHTML(files, path);
	if (closedir(dir_stream) == -1)
	{
		ServerError500(clt);
		return ;
	}

	clt->res->setResponseBody(html);

	// build content headers
	BuildContentHeaders(clt, "html", "");

	// add headers to the response
	std::string	headers = clt->res->returnMapAsString();
	if (headers.empty()) // stream error occurred
	{
		ServerError500(clt);
		return ;
	}
	response += headers;

	// add body to response
	response += clt->res->getResponseBody();
}
