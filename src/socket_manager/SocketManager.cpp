#include "SocketManager.hpp"

#include <unistd.h>

#define LISTEN_BACKLOG 10

SocketManager::SocketManager() {}

SocketManager::~SocketManager()
{
	std::vector<ServerSocket>::iterator it;
	for (it = servers_.begin(); it != servers_.end(); it++)
	{
		freeaddrinfo(it->ip_addr);
	}
}

enum SocketError SocketManager::set_servers(std::vector<std::pair<configuration::Socket,SocketConfiguration> > &socket_configs)
{
	assert(socket_configs.size() > 0);

	std::vector<std::pair<configuration::Socket,SocketConfiguration> >::iterator it;
	for (it = socket_configs.begin(); it != socket_configs.end(); it++)
	{
		struct addrinfo hints;
		struct addrinfo *res;
		int serv_sock;
		int yes = 1;
		int status;

		memset(&hints, 0, sizeof(hints));
		{
			/* code */
		}

	}

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
		client.socket = client_socket;
		client.server = get_one_server(server_socket);
		clients_.push_back(client);
		return (client_socket);
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

ssize_t SocketManager::send_all(int socket, std::string res_buf)
{
	int total_sent = 0;
	int
	while()
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