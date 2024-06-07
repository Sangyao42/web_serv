#include "Client.hpp"

void	client_lifespan::InitClient(struct Client &client, struct ClientSocket *client_socket)
{
	client.status_code = status_code::k200;
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
	client.status_code = status_code::k200;
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
	HeaderString	*Host = dynamic_cast<HeaderString *> (clt->req->returnValueAsPointer("Host"));

	// query configuration
	clt->config = &ws_database.query(clt->client_socket->socket, \
		(Host ? Host->content() : ""), \
		clt->req->getRequestTarget().path);

	if (clt->config->is_empty())
	{
		clt->status_code = k500;
		return ;
	}

	assert(clt->config->query && "Location Block in Server block is empty"); //query gives the all the needed info related to server block and location block
	cache::LocationQuery	*location= clt->config->query;
	if (!(location->allowed_methods & (int) clt->req->getMethod()))
	{
		clt->status_code = k405;
		return ;
	}

	HeaderInt *content_length = dynamic_cast<HeaderInt *> (clt->req->returnValueAsPointer("Content-Length"));

	if ((content_length && content_length->content() > location->client_max_body_size) || \
	(clt->req->body_size_chunked_ != -1 && clt->req->body_size_chunked_ > location->client_max_body_size))
	{
		clt->status_code = k413;
		return ;
	}


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

