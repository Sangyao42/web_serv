#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <vector>

#define MAX_SOCKETS 20

struct ClientSocket
{
	int socket;
	struct sockaddr_storage ip_addr;
	int &server_socket;
};

struct ServerSocket
{
	int socket;
	struct addrinfo ip_addr;
};

class SocketManager
{
	public:

	//getters and setters
	
	private:
		struct ServerSocket server;
		std::vector<struct ClientSocket> clients[MAX_SOCKETS];
};
