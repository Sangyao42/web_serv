#pragma once

#include "SocketError.hpp"
#include "../src/Configuration.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <poll.h>

#include <vector>
#include <iostream>

/**
 *
// All pointers to socket address structures are often cast to pointers
// to this type before use in various functions and system calls:

struct sockaddr {
    unsigned short    sa_family;    // address family, AF_xxx
    char              sa_data[14];  // 14 bytes of protocol address
};


// IPv4 AF_INET sockets:

struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET, AF_INET6
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};

struct in_addr {
    unsigned long s_addr;          // load with inet_pton()
};


// IPv6 AF_INET6 sockets:

struct sockaddr_in6 {
    u_int16_t       sin6_family;   // address family, AF_INET6
    u_int16_t       sin6_port;     // port number, Network Byte Order
    u_int32_t       sin6_flowinfo; // IPv6 flow information
    struct in6_addr sin6_addr;     // IPv6 address
    u_int32_t       sin6_scope_id; // Scope ID
};

struct in6_addr {
    unsigned char   s6_addr[16];   // load with inet_pton()
};
*/


// General socket address holding structure, big enough to hold either
// struct sockaddr_in or struct sockaddr_in6 data:
/**
 * struct sockaddr_storage {
    sa_family_t  ss_family;     // address family

    // all this is padding, implementation specific, ignore it:
    char      __ss_pad1[_SS_PAD1SIZE];
    int64_t   __ss_align;
    char      __ss_pad2[_SS_PAD2SIZE];
};
*/

//TEST: struct from configuration.hpp
// namespace configuration
// {
// 	struct Socket
// 	{
// 		std::string ip;
// 		std::string port;
// 		unsigned int   family_;
// 	};
// }

// struct SocketConfiguration
// {
// 	int server_socket;
// };

#define TIMEOUT 75

struct ClientSocket
{
	int socket;
	struct sockaddr_storage ip_addr;
	// int server_socket;
	struct ServerSocket server;
	std::string req_buf;
	std::string res_buf;

	Maybe<time_t> init_time;
	//time_t currernt_time;
	bool timeout;
};

// {
// 	Maybe<time_t> init_time;

// 	if (init_time.is_ok())
// 	{
// 		init_time.value()
// 	}
// 	else
// 	{
// 		init_time = 130;
// 	}
// }

//save the full linked list of res from getaddrinfo()
//use addr_to_bind to query the correct node in the linked list
struct ServerSocket
{
	int socket;
	struct addrinfo *add_info;
	int addr_to_bind;
};

class SocketManager
{
	public:

		SocketManager();
		~SocketManager();

		//methods
		int accept_client(int server_socket);
		ssize_t recv_append(int client_socket, char *buf);
		ssize_t send_all(int client_socket);
		//helpers
		void delete_client(int client_socket);
		//getters and setters
		enum SocketError set_servers(std::vector<std::pair<configuration::Socket,SocketConfiguration *> > socket_configs); //getaddrinfo(), socket(), bind(), listen()
		std::vector<struct ServerSocket> get_servers() const;
		std::vector<struct ClientSocket> * get_clients();
		struct ServerSocket get_one_server(int server_socket) const; //query server by socket
		struct ClientSocket *get_one_client(int client_socket); //query client by socket
		struct addrinfo *get_server_addrinfo(int server_socket);

		//update for managing client timeout
		void	update_init_time(int client_socket);
		void	update_timeout(ClientSocket *client);
		bool	is_timeout(int client_socket);
	private:
		std::vector<struct ServerSocket> servers_;
		std::vector<struct ClientSocket> clients_;

		SocketManager(const SocketManager &src);
		SocketManager &operator=(const SocketManager &src);
};
