#pragma once

#include <iterator>
#include <fstream>
#include <sstream>
#include <ctime>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Request.hpp"
#include "Response.hpp"
#include "Protocol.hpp"
#include "SocketManager.hpp"
#include "Configuration.hpp"

enum ResponseError
{
	kNoError,
	kFileOpenError,
	kFilestreamError
};

struct Client
{
	status_code	status_code;
	struct ClientSocket	*client_socket;
	const struct ConfigurationQueryResult	*config;
	struct stat	stat_buff;
	std::string path;
	std::vector<std::string> cgi_argv; //path to cgi executable and path to cgi script
	std::vector<std::string> cgi_env;
	bool	keepAlive = true;
	Request	*req;
	Response	*res;
};

namespace process
{
	void	ProcessRequest(struct Client *clt);
	void	ProcessGetRequest(struct Client *clt);
	void	ProcessPostRequest(struct Client *clt);
	void	ProcessDeleteRequest(struct Client *clt);

	void	ProcessGetRequestCgi(struct Client *clt);
	void	ProcessPostRequestCgi(struct Client *clt);

	//file and path and content-type related functions
	std::string GetExactPath(const std::string root, std::string match_path, const struct Uri uri);
	bool		IsCgi(std::vector<std::string> &cgi_executable, std::string path, cache::LocationQuery *location);
	std::string	GetResContentType(std::string path);
	bool		IsAccessable(std::string content_type, HeaderValue *accept, cache::LocationQuery *location);
	std::string	GetIndexPath(std::string path, cache::LocationQuery *location);
	bool		IsSupportedMediaType(std::string req_content_type, const directive::MimeTypes* mime_types);
	bool		IsDirFormat(std::string path);


	//cgi related functions
	enum PipeEnd
	{
		kRead,
		kWrite
	};
	int		SetPipes(int *cgi_input, int *cgi_output, const Method method);
	std::vector<char *>	ConstructExecArray(std::vector<std::string> &cgi_params);
	void	SetCgiEnv(struct Client *clt);
	int	ReadAll(int fd, std::string &response_tmp);

	//helper functions
	int	StringVecToTwoDimArray(std::vector<char *> &cstring, const std::vector<std::string> &strings);
}

namespace res_builder
{
	void	GenerateErrorResponse(struct Client *clt);
	void	GenerateRedirectResponse(struct Client *clt);
	void	GenerateAutoindexResponse(struct Client *clt);
	void	GenerateSuccessResponse(struct Client *clt);

	// error page related helper functions
	void	BuildErrorHeaders(struct Client *clt);
	const std::string &BuildErrorPage(enum status_code code);

	// redirect related helper functions

	// autoindex related helper functions

	// success related helper functions

	// general utility functions
	void	BuildContentHeaders(struct Client *clt);
	void	ServerError(struct Client *clt);
	std::string	GetTimeGMT();
	void	BuildBasicHeaders(Response *res);
	void	BuildStatusLine(enum status_code status_code, std::string &response);
	enum ResponseError	ReadFileToString(const std::string &path, std::string &body);
	const std::string	&StatusCodeAsString(enum status_code code);
}
