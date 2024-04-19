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

// generateErrorResponse()
// ...

// buildStatusLine()
// buildHeaders()

const std::string	&statusCodeAsString(enum StatusCode code);