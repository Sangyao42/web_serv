#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include "ResponseStatus.hpp"
#include "SocketManager.hpp"
#include "Configuration.hpp"


class Client
{
	public:
		Client();
		Client(const Client &obj);
		virtual ~Client();

		Client &operator=(const Client &obj);

		generateErrorResponse
		...

	private:
		StatusCode	statusCode_;
		struct ClientSocket	*clientSocket_;
		struct ConfigurationQueryResult	*conf_;
		Request	*request_;
		Response	*response_;

		buildStatusLine
		buildHeaders
		concatenate
};