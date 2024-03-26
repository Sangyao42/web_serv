#pragma once

#include "SocketError.hpp"
// #include "configuration.hpp"
// #include "SocketConfiguration.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <poll.h>

#include <vector>
#include <iostream>

/**
 * struct addrinfo {
    int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    int              ai_protocol;  // use 0 for "any"
    size_t           ai_addrlen;   // size of ai_addr in bytes
    struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
    char            *ai_canonname; // full canonical hostname

    struct addrinfo *ai_next;      // linked list, next node
};
*/

/**
 * struct sockaddr_in {
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Same size as struct sockaddr
};
*/

/**
 *struct sockaddr_in6 {
    u_int16_t       sin6_family;   // address family, AF_INET6
    u_int16_t       sin6_port;     // port number, Network Byte Order
    u_int32_t       sin6_flowinfo; // IPv6 flow information
    struct in6_addr sin6_addr;     // IPv6 address
    u_int32_t       sin6_scope_id; // Scope ID
};

struct in6_addr {
    unsigned char   s6_addr[16];   // IPv6 address
};
*/

/**
 * struct sockaddr_storage {
    sa_family_t  ss_family;     // address family

    // all this is padding, implementation specific, ignore it:
    char      __ss_pad1[_SS_PAD1SIZE];
    int64_t   __ss_align;
    char      __ss_pad2[_SS_PAD2SIZE];
};
*/
struct ClientSocket
{
	int socket;
	struct sockaddr_storage ip_addr;
	// int server_socket;
	struct ServerSocket *server;
	std::string req_buf;
	std::string res_buf;
};

struct ServerSocket
{
	int socket;
	struct addrinfo *ip_addr;
};

class SocketManager
{
	public:

		SocketManager();
		~SocketManager();

		//methods
		int accept_client(int server_socket);
		ssize_t recv_append(int client_socket, char *buf);
		ssize_t send_all(int socket, std::string res_buf);
		//helpers
		void delete_client(int client_socket);
		//getters and setters
		enum SocketError set_servers(std::vector<std::pair<configuration::Socket,SocketConfiguration *> > socket_configs); //getaddrinfo(), socket(), bind(), listen()
		std::vector<struct ServerSocket> & get_servers() const;
		std::vector<struct ClientSocket> & get_clients();
		struct ServerSocket *get_one_server(int server_socket) const; //query server by socket
		struct ClientSocket *get_one_client(int client_socket); //query client by socket
	private:
		std::vector<struct ServerSocket> servers_;
		std::vector<struct ClientSocket> clients_;

		SocketManager(const SocketManager &src);
		SocketManager &operator=(const SocketManager &src);
};
