#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <vector>
#include <iostream>

struct ClientSocket
{
	int socket;
	struct sockaddr_storage ip_addr;
	int &server_socket;
	std::string req_buf;
	std::string res_buf;
};

struct ServerSocket
{
	int socket;
	struct addrinfo ip_addr;
};

class SocketManager
{
	public:

	SocketManager();
	~SocketManager();

	//methods
	ssize_t recv_append(int socket, char *buf);
	ssize_t send_all(int socket, char *buf, ssize_t *len);
	//getters and setters
	int	set_servers(std::vector<int> serv_ports);
	std::vector<struct ServerSocket> & get_servers();
	int	get_clients();
	struct ClientSocket get_one_client(int socket);
	private:
		std::vector<struct ServerSocket> servers_;
		std::vector<struct ClientSocket> clients_;
};
