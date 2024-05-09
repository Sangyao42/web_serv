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
}

struct Client
{
	StatusCode	statusCode;
	struct ClientSocket	*clientSocket;
	const struct ConfigurationQueryResult	*config;
	struct stat	statBuff;
	std::string path;
	bool	keepAlive = true;
	Request	*req;
	Response	*res;
};

namespace process
{
	void	processRequest(struct Client *clt);
	void	processGetRequest(struct Client *clt);
	void	processPostRequest(struct Client *clt);
	void	processDeleteRequest(struct Client *clt);

	std::string getExactPath(const std::string root, std::string matchPath, const std::string requestTarget);
}

namespace resBuilder
{
	void	generateErrorResponse(struct Client *clt);
	void	generateRedirectResponse(struct Client *clt);
	void	generateChunkedResponse(struct Client *clt);
	void	generateAutoindexResponse(struct Client *clt);
	void	generateSuccessResponse(struct Client *clt);

	namespace helper
	{
		void	buildErrorHeaders(struct Client *clt);
		enum ResponseBuilder	buildErrorPage(const struct Client *clt);
	}

	namespace utils
	{
		void	buildStatusLine(const struct Client *clt, std::string &response);
		const std::string &readFileToString(const std::string &path);
		const std::string	&statusCodeAsString(enum StatusCode code);
	}

}
