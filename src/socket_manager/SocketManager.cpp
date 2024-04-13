#include "SocketManager.hpp"

#include "misc/Maybe.hpp"
#include "Uri/Authority.hpp"
#include "Configuration.hpp"

#include <unistd.h>
#include <fcntl.h>

#define LISTEN_BACKLOG 10

SocketManager::SocketManager() {}

SocketManager::~SocketManager()
{
	std::vector<ServerSocket>::iterator it;
	for (it = servers_.begin(); it != servers_.end(); it++)
	{
		close(it->socket);
		if (it->add_info != NULL)
			freeaddrinfo(it->add_info);
	}
}

//setter for servers_, which uses getaddrinfo(), socket(), bind(), listen() to set up the server sockets
//This means server.start()
enum SocketError SocketManager::set_servers(std::vector<const uri::Authority*> socket_configs)
{
	assert(socket_configs.size() > 0);

	struct addrinfo hints;
	int serv_sock;
	int status;

	std::vector<const uri::Authority*>::const_iterator it;
	for (it = socket_configs.begin(); it != socket_configs.end(); it++)
	{
		//for setsockopt()
		int yes = 1;
		struct addrinfo *res;
		int res_len = -1;

		//set socketaddress hints
		memset(&hints, 0, sizeof(hints));
		if ((*it)->family() == uri::Host::IPV4)
			hints.ai_family = AF_INET;
		else if ((*it)->family() == uri::Host::IPV6)
			hints.ai_family = AF_INET6;
		else
			hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		if ((*it)->family() == uri::Host::REGNAME)
		{
			hints.ai_flags = AI_PASSIVE;
			status = getaddrinfo(NULL, (*it)->port.c_str(), &hints, &res);
		}
		else
			status = getaddrinfo((*it)->host.value.c_str(), (*it)->port.c_str(), &hints, &res);
		if (status != 0)
		{
			std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
			return (kGetAddrInfoError);
		}
		struct addrinfo *ai_ptr;
		for (ai_ptr = res; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next)
		{
			res_len++;
			serv_sock = socket(ai_ptr->ai_family, ai_ptr->ai_socktype, ai_ptr->ai_protocol);
			if (serv_sock == -1)
			{
				std::cerr << "socket: " << strerror(errno) << std::endl;
				continue;
			}
			if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
			{
				std::cerr << "setsockopt: " << strerror(errno) << std::endl;
				close(serv_sock);
				freeaddrinfo(res);
				return (kSetSockOptError);
			}
			if (fcntl(serv_sock, F_SETFL, O_NONBLOCK) == -1)
			{
				std::cerr << "fcntl: " << strerror(errno) << std::endl;
				close(serv_sock);
				freeaddrinfo(res);
				return (kFcntlError);
			}
			if (bind(serv_sock, ai_ptr->ai_addr, ai_ptr->ai_addrlen) == -1)
			{
				std::cerr << "bind: " << strerror(errno) << std::endl;
				close(serv_sock);
				continue;
			}
			break;
		}
		if (ai_ptr == NULL)
		{
			std::cerr << "bind: failed to bind" << std::endl;
			freeaddrinfo(res);
			return (kBindError);
		}
		if (listen(serv_sock, LISTEN_BACKLOG) == -1)
		{
			std::cerr << "listen: " << strerror(errno) << std::endl;
			close(serv_sock);
			freeaddrinfo(res);
			return (kListenError);
		}
		//free the addrinfo struct and close the socket fd in SocketManager destructor
		ServerSocket server;
		server.socket = serv_sock;
		server.add_info = res;
		server.addr_to_bind = res_len;
		servers_.push_back(server);
		ws_database.register_server_socket(serv_sock, **it);
	}
	return (kNoError);
}

int SocketManager::accept_client(int server_socket)
{
	ClientSocket client;
	socklen_t addrlen;
	addrlen = sizeof(client.ip_addr);

	int client_socket = accept(server_socket, (struct sockaddr *)&client.ip_addr, &addrlen);
	if (client_socket == -1)
	{
		std::cerr << "accept: " << strerror(errno) << std::endl;
		return (-1);
	}
	else
	{
		if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
		{
			std::cerr << "fcntl: client socket" << strerror(errno) << std::endl;
			return (-1);
		}
		else
		{
			client.socket = client_socket;
			client.server = get_one_server(server_socket);
			client.last_active = time(NULL);
			client.first_recv_time = Maybe<time_t>();
			client.timeout = false;
			clients_.push_back(client);
			std::cout << "accept: client connected with fd " << client_socket << std::endl;
			return (client_socket);
		}
	}
}

ssize_t SocketManager::recv_append(int client_socket, char *buf)
{
	ssize_t recv_len;
	recv_len = recv(client_socket, buf, sizeof(buf), 0);
	if (recv_len <= 0)
	{
		if (recv_len < 0)
		{
			std::cerr << "recv: " << strerror(errno) << std::endl;
		}
		else
		{
			std::cout << "recv: client disconnected" << std::endl;
		}
		delete_client(client_socket);
	}
	else
	{
		ClientSocket *client = get_one_client(client_socket);
		client->req_buf.append(buf, recv_len);
	}
	return (recv_len);
}

ssize_t SocketManager::send_all(int client_socket)
{
	ClientSocket *client = get_one_client(client_socket);

	//std::string res_buf = client->res_buf;
	ssize_t total_sent = 0; //bytes have been sent
	ssize_t bytes_left = client->res_buf.size(); //bytes left to send
	ssize_t sent_bytes = 0; //bytes sent in one send() call

	while(bytes_left > 0)
	{
		sent_bytes = send(client_socket, client->res_buf.c_str() + total_sent, bytes_left, 0);
		if (sent_bytes == -1)
		{
			std::cerr << "send: " << strerror(errno) << std::endl;
			break;
		}
		bytes_left -= sent_bytes;
		total_sent += sent_bytes;
	}
	//TODO: do I need to close the client socket if send fails?
	if (sent_bytes == -1)
	{
		delete_client(client_socket);
		return (-1);
	}
	else
	{
		client->res_buf.clear();
	}
	return (total_sent);
}

void SocketManager::delete_client(int client_socket)
{
	std::vector<ClientSocket>::iterator it;
	for (it = clients_.begin(); it != clients_.end(); it++)
	{
		if (it->socket == client_socket)
		{
			clients_.erase(it);
			break;
		}
	}
}

//getters
std::vector<struct ServerSocket> SocketManager::get_servers() const
{
	return (servers_);
}

std::vector<struct ClientSocket> *SocketManager::get_clients()
{
	return (&clients_);
}

struct ServerSocket SocketManager::get_one_server(int server_socket) const
{
	std::vector<ServerSocket>::const_iterator it;
	for (it = servers_.begin(); it != servers_.end(); it++)
	{
		if (it->socket == server_socket)
		{
			return (*it);
		}
	}
	std::cerr << "get_one_server: server not found" << std::endl;
	ServerSocket server;
	server.socket = -1;
	return (server);
}

struct ClientSocket *SocketManager::get_one_client(int client_socket)
{
	std::vector<ClientSocket>::iterator it;
	for (it = clients_.begin(); it != clients_.end(); it++)
	{
		if (it->socket == client_socket)
		{
			return (&(*it));
		}
	}
	std::cerr << "get_one_client: client not found" << std::endl;
	return (NULL);
}

struct addrinfo *SocketManager::get_server_addrinfo(int server_socket)
{
	struct addrinfo *ai_ptr;

	std::vector<ServerSocket>::iterator it;
	for (it = servers_.begin(); it != servers_.end(); it++)
	{
		if (it->socket == server_socket)
		{
			int ai_index = 0;
			for (ai_ptr = it->add_info; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next)
			{
				if (ai_index == it->addr_to_bind)
					return (ai_ptr);
				ai_index++;
			}
		}
	}
	std::cerr << "get_server_addrinfo: server not found" << std::endl;
	return (NULL);
}

//udpate for managing client timeouts
//set first_recv_time to time of first recv of one request
void	SocketManager::set_first_recv_time(int client_socket)
{
	ClientSocket *client = get_one_client(client_socket);
	if (client->first_recv_time.is_ok() == false)
	{
		client->first_recv_time = time(NULL); //set the time and set is_ok to true
	}
}

bool	SocketManager::is_timeout(int client_socket)
{
	ClientSocket *client = get_one_client(client_socket);
	update_timeout(client);
	return (client->timeout);
}

void	SocketManager::update_timeout(ClientSocket *client)
{
	//timeout when complete request not received within TIMEOUT
	if (client->first_recv_time.is_ok() == true)
	{
		time_t current_time = time(NULL);
		if (current_time - client->first_recv_time.value() > TIMEOUT * 1000)
		{
			client->timeout = true;
		}
	}
	//timeout when no new request received after last response sent
	else
	{
		time_t current_time = time(NULL);
		if (current_time - client->last_active > TIMEOUT * 1000)
		{
			client->timeout = true;
		}
	}
}

void	SocketManager::set_time_assets(int client_socket)
{
	ClientSocket *client = get_one_client(client_socket);
	client->last_active = time(NULL);
	client->first_recv_time = Maybe<time_t>();
	client->timeout = false;
}
