#include "Client.hpp"

void res_builder::BuildJsonBody(struct Client *clt)
{
	std::string response = "{\n";
	response += "\"status\": " + std::to_string(clt->status_code) + ",\n";
	response += "\"message\": \"" + StatusCodeAsString(clt->status_code) + "\"\n";
	response += "}\n";
	clt->res->setBody(response);
}

void	res_builder::GenerateSuccessResponse(struct Client *clt)
{
	// build the status line
	std::string response = clt->client_socket->res_buf;
	BuildStatusLine(clt->status_code, response);

	// build basic headers
	BuildBasicHeaders(clt->res);

	// build the body and content headers
	if (!clt->cgi_argv.empty())
	{
		// is cgi request
		// do something
	}
	else
	{
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

			AddAllowHeader(clt);

			clt->res->addNewPair("Location", new HeaderString(clt->location_created));
			// size_t root_pos = file_path.find(clt->config->query->root) + clt->config->query->root.size() - 1;
			// clt->location_created = file_path.substr(root_pos);
		}
	}
}