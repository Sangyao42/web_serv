#include "Client.hpp"
#include "Protocol.hpp"

#include <cstring>
#include <cassert>

extern const size_t	constants::kDefaultClientMaxBodySize;


void	client_lifespan::InitClient(struct Client &client, struct ClientSocket *client_socket)
{
	client.status_code = k000;
	client.client_socket = client_socket;
	client.config.location_block = NULL;
	client.config.query = NULL;
	//???? do I need to set stat_buff to 0???
	memset(&client.stat_buff, 0, sizeof(struct stat));
	client.keepAlive = true;
	client.content_length = 0;
	client.max_body_size = constants::kDefaultClientMaxBodySize;
	client.continue_reading = false;
	client.exceed_max_body_size = false;
	client.is_chunked = false;
	client.is_chunk_end = false;
	client.consume_body = true;
	new (&client.req) Request();
	new (&client.res) Response();
}

void	client_lifespan::ResetClient(struct Client &client)
{
	client.status_code = k000;
	client.client_socket->res_buf.clear();
	client.config.location_block = NULL;
	client.config.query = NULL;
	memset(&client.stat_buff, 0, sizeof(struct stat));
	client.path.clear();
	client.cgi_argv.clear();
	client.cgi_env.clear();
	client.keepAlive = true;
	client.content_length = 0;
	client.max_body_size = constants::kDefaultClientMaxBodySize;;
	client.continue_reading = false;
	client.exceed_max_body_size = false;
	client.is_chunked = false;
	client.is_chunk_end = false;
	client.consume_body = true;
	client.location_created.clear();
	client.cgi_content_type.clear();
	client.cgi_content_length = 0;
	client.req.cleanHeaderMap();
	client.res.cleanHeaderMap();
	new (&client.req) Request();
	new (&client.res) Response();
}

int	client_lifespan::DeleteClientFromVector(std::vector<struct Client> &clients, int client_fd)
{
  int index = -1;
	std::vector<struct Client>::iterator it = clients.begin();
	for (it = clients.begin(); it != clients.end(); it++)
	{
		if (it->client_socket->socket == client_fd)
		{
      index = it - clients.begin();
			clients.erase(it);
			break;
		}
	}
  return index;
}

//update functions
// void client_lifespan::UpdateStatusCode(struct Client clt, StatusCode statuscode)
// {
// 	clt.status_code = statuscode;
// }

//check if client is alive
bool client_lifespan::IsClientAlive(struct Client *clt)
{
	return (clt->keepAlive);
}

void	client_lifespan::CheckHeaderBeforeProcess(struct Client *clt)
{
	// check if is_chunked
	HeaderString	*transfer_encoding = static_cast<HeaderString *> (clt->req.returnValueAsPointer("Transfer-Encoding"));
	if (transfer_encoding && transfer_encoding->content() == "chunked")
	{
		clt->is_chunked = true;
	}

	std::string requestline_host = clt->req.request_target_.authority.host.value;
	if (requestline_host == "")
	{
		HeaderString	*header_host = static_cast<HeaderString *> (clt->req.returnValueAsPointer("Host"));
		// assert(header_host && "Host header is missing");
		// requestline_host = header_host->content();
		// assert((requestline_host != "") && "Host header is empty");
		if (!header_host || header_host->content() == "")
		{
			clt->status_code = k400;
			clt->consume_body = false;
			return ;
		}
	}
	// query configuration
	clt->config = ws_database.query(clt->client_socket->server.socket, \
		requestline_host, clt->req.getRequestTarget().path);

	assert(clt->config.query && "No configuration found for this request");

	assert(clt->config.query && "Location Block in Server block is empty"); //query gives the all the needed info related to server block and location block
	cache::LocationQuery	*location= clt->config.query;

	clt->max_body_size = location->client_max_body_size;

	if (!(location->allowed_methods & (int) clt->req.getMethod()))
	{
		if (clt->status_code == k000)
		{
			clt->status_code = k405;
			clt->consume_body = false;
			return ;
		}
	}

	// check if the path exists (for get and delete)
	std::string path = process::GetExactPath(location->root, location->match_path, clt->req.getRequestTarget());
	clt->path = path;
	if (clt->req.getMethod() == kGet || clt->req.getMethod() == kDelete)
	{
		if (access(path.c_str(), F_OK) != 0)
		{
			if (clt->status_code == k000)
			{
				clt->status_code = k404;
				clt->consume_body = false;
				return ;
			}
		}
		if ((clt->req.getMethod() == kGet) && (access(path.c_str(), R_OK) != 0))
		{
			if (clt->status_code == k000)
			{
				clt->status_code = k403;
				clt->consume_body = false;
				return ;
			}
		}
		if (stat(path.c_str(), &clt->stat_buff) != 0)
		{
			if (clt->status_code == k000)
			{
				write(2, "stat error\n", 11);
				clt->status_code = k500;
				clt->consume_body = false;
				return ;
			}
		}
	}

	//For POST with Content-Length (normal request, without chunks)
	HeaderInt *content_length = static_cast<HeaderInt *>(clt->req.returnValueAsPointer("Content-Length"));
	if ((clt->req.getMethod() == kPost) && !content_length && !clt->is_chunked)
	{
		clt->status_code = k411;
		clt->consume_body = false;
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

