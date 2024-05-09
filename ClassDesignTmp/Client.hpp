#pragma once

#include <iterator>
#include <fstream>
#include <sstream>
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
	StatusCode	status_code;
	struct ClientSocket	*client_socket;
	const struct ConfigurationQueryResult	*config;
	struct stat	stat_buff;
	std::string path;
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

	//file and path related functions
	std::string GetExactPath(const std::string root, std::string matchPath, const struct Uri uri);
	bool	IsCgi(std::string path);
	bool	IsAccessable(std::string path);
	std::string	GetIndexPath();
}

namespace resBuilder
{
	void	GenerateErrorResponse(struct Client *clt);
	void	GenerateRedirectResponse(struct Client *clt);
	void	GenerateChunkedResponse(struct Client *clt);
	void	GenerateAutoindexResponse(struct Client *clt);
	void	GenerateSuccessResponse(struct Client *clt);

	namespace helper
	{
		void	BuildErrorHeaders(struct Client *clt);
		enum ResponseBuilder	BuildErrorPage(const struct Client *clt);
	}

	namespace utils
	{
		void	BuildStatusLine(const struct Client *clt, std::string &response);
		const std::string &ReadFileToString(const std::string &path);
		const std::string	&StatusCodeAsString(enum StatusCode code);
	}

}
