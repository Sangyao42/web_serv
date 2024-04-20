#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include "Protocol.hpp"
// #include "SocketManager.hpp"
// #include "Configuration.hpp"

struct Client
{
	StatusCode	statusCode;
	struct ClientSocket	*clientSocket;
	struct ConfigurationQueryResult	*config;
	Request	*req;
	Response	*res;
};

void	generateErrorResponse(struct Client clt);
void	generateRedirectResponse(struct Client clt);
void	generateChunkedResponse(struct Client clt);
void	generateAutoindexResponse(struct Client clt);
void	generateFileResponse(struct Client clt);

const std::string	&statusCodeAsString(enum StatusCode code);