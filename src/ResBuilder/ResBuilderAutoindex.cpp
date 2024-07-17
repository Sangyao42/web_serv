#include "Client.hpp"
#include <cassert>

std::string res_builder::BuildAutoindexHTML(DSet files, std::string path)
{
	DSetIt it;

	std::string html = "<!DOCTYPE html>\r\n";
	html += "<html>\r\n";
	html += "<head>";

    // link to font-awesome
    // html += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css\">";
    html += "<style>";

    // CSS
    html += "* { margin: 0; padding: 0; outline: 0; }";
    html += "body { padding: 80px 100px; font-family: Arial, sans-serif; font-size: 15px; background-color: #eeede7; }";
    html += "h1 { margin: 25px 3px; font-size: 30px; color : #474440; }";
    html += "ul { display: flex; flex-wrap: wrap; }";
    html += "ul li { min-width: 200px; padding-left: 2px; width: 30%; line-height: 25px; list-style: none; }";
    html += "a { text-decoration: none; color: #555; }";
    html += "ul li a { padding: 5px; width: auto; display: block; height: 25px; border-radius: 5px; }";
    html += "ul li a:hover { background-color: rgba(255, 255, 255, 0.65); border: 1px solid #ececec; }";
    html += "i { padding: 3px; margin-right: 8px; }";

	html += "</style><title>Index of ";
	html += path;
	html += "</title></head>\r\n";
	html += "<body>\r\n";

    // TODO: add links to the path
	html += "<h1>Index of ";
	html += path;
	html += "</h1>\r\n";
	html += "<ul>";


	if (!path.empty() && path[path.size() - 1] != '/')
		path = path + "/";

	// add icons and links for directories and files
    for (it = ++(files.begin()); it != files.end(); ++it)
    {
      html += "<li><a href=\"";
      html += path + it->second;
      if (it->first == DT_DIR)
        html += "\"><i class=\"fa-regular fa-folder\"></i>";
      else
        html += "\"><i class=\"fa-regular fa-file\"></i>";
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
	std::string &response = clt->client_socket->res_buf;
	BuildStatusLine(clt->status_code, response);

	// build basic headers
	BuildBasicHeaders(&clt->res);

	// build autoindex body
	struct dirent *dirent;
	std::string path = clt->path;
	DIR	*dir_stream = opendir(path.c_str());

	if (dir_stream == NULL)
	{
		assert((errno == ENOENT || errno == ENOTDIR || errno == EACCES) && "Unexpected error opening directory");
		ServerError500(clt);
		return ;
	}

	DSet files;

	while ((dirent = readdir(dir_stream)) != NULL)
	{
		files.insert(std::make_pair(dirent->d_type, dirent->d_name));
	}

	// if (errno != 0)
	// {
	// 	ServerError500(clt);
	// 	closedir(dir_stream);
	// 	return ;
	// }

	std::string html = BuildAutoindexHTML(files, clt->req.getRequestTarget().path);
	if (closedir(dir_stream) == -1)
	{
		ServerError500(clt);
		return ;
	}

	clt->res.setResponseBody(html);

	// build content headers
	BuildContentHeaders(clt, "text/html", "");

	// add headers to the response
	std::string	headers = clt->res.returnMapAsString();
	if (headers.empty()) // stream error occurred
	{
		ServerError500(clt);
		return ;
	}
	response += headers;

	// add body to response
	response += clt->res.getResponseBody();
}
