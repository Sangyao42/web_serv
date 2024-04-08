#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include "ResponseStatus.hpp"
#include "SocketManager.hpp"
#include "Configuration.hpp"

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
