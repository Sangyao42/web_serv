#include "Client.hpp"

void	client_lifespan::InitClient(struct Client &client, struct ClientSocket *client_socket)
{
	client.status_code = status_code::k000;
	client.client_socket = client_socket;
	client.config = NULL;
	//???? do I need to set stat_buff to 0???
	memset(&client.stat_buff, 0, sizeof(struct stat));
	client.keepAlive = true;
	continue_reading = false;
	exceed_max_body_size = false;
	is_chunked = false;
	consume_body = true;
	new (&client.Req) Request();
	new (&client.Res) Response();
}

void	client_lifespan::ResetClient(struct Client &client)
{
	client.status_code = status_code::k000;
	client.client_socket->res_buf.clear();
	client.config = NULL;
	memset(&client.stat_buff, 0, sizeof(struct stat));
	client.path.clear();
	client.cgi_argv.clear();
	client.cgi_env.clear();
	client.keepAlive = true;
	continue_reading = false;
	exceed_max_body_size = false;
	is_chunked = false;
	consume_body = true;
	client.location_created.clear();
	client.cgi_content_type.clear();
	client.cgi_content_length.clear();
	req.cleanHeaderMap();
	new (&client.Req) Request();
	new (&client.Res) Response();
}

void	client_lifespan::DeleteClientFromVector(std::vector<struct Client> &clients, int client_fd)
{
	std::vector<struct Client>::iterator it;
	for (it = clients.begin(); it != clients.end(); it++)
	{
		if (it->client_socket->socket == client_fd)
		{
			clients.erase(it);
			break;
		}
	}
}

//update functions
void client_lifespan::UpdateStatusCode(struct Client &clt, status_code statuscode)
{
	clt->status_code = statuscode;
}

//check if client is alive
bool client_lifespan::IsClientAlive(struct Client &clt)
{
	return (clt.keepAlive);

}

void	client_lifespan::CheckHeaderBeforeProcess(struct Client *clt)
{
	// there is an existing error
	if (clt->status_code != k000)
		return ;

	HeaderString	*Host = dynamic_cast<HeaderString *> (clt->req->returnValueAsPointer("Host"));
	// query configuration
	clt->config = &ws_database.query(clt->client_socket->socket, \
		(Host ? Host->content() : ""), \
		clt->req->getRequestTarget().path);

	if (clt->config->is_empty())
	{
		clt->status_code = k500;
		clt->consume_body = false;
		return ;
	}

	assert(clt->config->query && "Location Block in Server block is empty"); //query gives the all the needed info related to server block and location block
	cache::LocationQuery	*location= clt->config->query;
	if (!(location->allowed_methods & (int) clt->req->getMethod()))
	{
		clt->status_code = k405;
		clt->consume_body = false;
		return ;
	}

	// check if the path exists (for get and delete)
	std::string path = process::GetExactPath(location->root, location->match_path, clt->req->getRequestTarget());
	clt->path = path;
	if ((access(path.c_str(), F_OK) != 0) \
	&& (clt->req->getMethod() == kGet || clt->req->getMethod() == kDelete))
	{
		clt->status_code = k404;
		clt->consume_body = false;
		return ;
	}
	else
	{
		if (stat(path.c_str(), &clt->stat_buff) != 0)
		{
			clt->status_code = k500;
			clt->consume_body = false;
			return ;
		}
	}

	// check if is_chunked
	HeaderString	*transfer_encoding = dynamic_cast<HeaderString *> (clt->req->returnValueAsPointer("Transfer-Encoding"));
	if (transfer_encoding && transfer_encoding->content() == "chunked")
	{
		clt->is_chunked = true;
		return ;
	}

	return ;
}

//helper functions
struct Client	*client_lifespan::GetClientByFd(std::vector<struct Client> &clients, int client_fd)
{
	std::vector<struct Client>::iterator it;
	for (it = clients.begin(); it != clients.end(); it++)
	{
		if (it->client_socket->socket == client_fd)
		{
			return (&(*it));
		}
	}
	return (NULL);
}

