#pragma once

#include <iterator>
#include <fstream>
#include <sstream>
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
	struct ConfigurationQueryResult	*config;
	struct stat	statBuff;
	std::string path;
	Request	*req;
	Response	*res;
};

namespace resBuilder
{
	void	generateErrorResponse(struct Client *clt);
	void	generateRedirectResponse(struct Client *clt);
	void	generateChunkedResponse(struct Client *clt);
	void	generateAutoindexResponse(struct Client *clt);
	void	generateSuccessResponse(struct Client *clt);

	namespace helper
	{
		void	buildErrorHeaders(struct Client *clt)
		enum ResponseBuilder	buildErrorPage(const struct Client *clt)
	}

	namespace utils
	{
		void	buildStatusLine(const struct Client *clt, std::string &response)
		const std::string &readFileToString(const std::string &path)
		const std::string	&statusCodeAsString(enum StatusCode code);
	}

}
