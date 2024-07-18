#pragma once

#include "SocketError.hpp"
#include "Configuration.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <poll.h>

#include <vector>
#include <iostream>

#define TIMEOUT 75
#define BUF_SIZE 1024

//save the full linked list of res from getaddrinfo()
//use addr_to_bind to query the correct node in the linked list
struct ServerSocket
{
	int socket;
	struct addrinfo *add_info;
	int addr_to_bind;
};

struct ClientSocket
{
	int socket;
	struct sockaddr_storage ip_addr;
	// int server_socket;
	struct ServerSocket server;
	std::string req_buf;
	std::string res_buf;

	//for checking if body is received completely
	// bool body_leftover; // ? do I need to check it?

	//for managing client timeout
	time_t last_active; //accept time or last request time
	Maybe<time_t> first_recv_time; //time of first recv of one request
	bool timeout;
};

class SocketManager
{
	public:

		SocketManager();
		SocketManager(int max_clients);
		~SocketManager();

		//methods
		int accept_client(int server_socket);
		ssize_t recv_append(int client_socket, char *buf);
		ssize_t send_all(int client_socket);
		//helpers
		void delete_client_socket(int client_socket);
		//getters and setters
		enum SocketError set_servers(std::vector<const uri::Authority*> socket_configs); //getaddrinfo(), socket(), bind(), listen()
		std::vector<struct ServerSocket> get_servers() const;
		std::vector<struct ClientSocket> * get_clients();
		struct ServerSocket get_one_server(int server_socket) const; //query server by socket
		struct ClientSocket *get_one_client(int client_socket); //query client by socket
		struct addrinfo *get_server_addrinfo(int server_socket);

		//update for managing client timeout
		void	set_first_recv_time(int client_socket);
		void	update_timeout(ClientSocket *client);
		bool	is_timeout(int client_socket);
		void	set_time_assets(int client_socket);
	private:
		std::vector<struct ServerSocket> servers_;
		std::vector<struct ClientSocket> clients_;

		SocketManager(const SocketManager &src);
		SocketManager &operator=(const SocketManager &src);
};
