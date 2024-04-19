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
	Request	*request;
	Response	*response;
}

// generateErrorResponse()
// ...

// buildStatusLine()
// buildHeaders()

std::string	statusCodeAsString(enum StatusCode code);