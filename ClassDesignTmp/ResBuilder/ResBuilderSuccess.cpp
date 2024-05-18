#include "Client.hpp"

void	res_builder::GenerateSuccessResponse(struct Client *clt)
{
	// build the status line
	std::string response = clt->client_socket->res_buf;
	BuildStatusLine(clt->status_code, response);

	// build basic headers
	BuildBasicHeaders(clt->res);

	// build the body
	if (clt->req->getMethod() == kGet) // it is not a cgi request
	{
		if (ReadFileToBody(clt->path, clt->res) != kNoError)
		{
			ServerError500(clt);
			return ;
		}
		BuildContentHeaders(clt, clt->path);
	}

	if (clt->req->getMethod() == kPost) // it is not a cgi request
	{
		// build content headers
		//

		// ? if i have to query location again
		// + Allow header
		// clt->config->query->allowed_methods;
		// clt->res->addNewPair("Allow", new HeaderSomething());

		// + Location header
		clt->res->addNewPair("Location", new HeaderString(clt->location_created));
		// size_t root_pos = file_path.find(clt->config->query->root) + clt->config->query->root.size() - 1;
		// clt->location_created = file_path.substr(root_pos);
	}
}