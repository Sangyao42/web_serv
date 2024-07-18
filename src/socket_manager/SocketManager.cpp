#include "SocketManager.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include <cerrno>
#include <cassert>

#include "misc/Maybe.hpp"
#include "Uri/Authority.hpp"
#include "Configuration.hpp"

#define LISTEN_BACKLOG 10

SocketManager::SocketManager() {}

SocketManager::SocketManager(int max_clients)
{
	clients_.reserve(max_clients);
}

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

// setter for servers_, which uses getaddrinfo(), socket(), bind(), listen() to set up the server sockets
// This means server.start()
enum SocketError SocketManager::set_servers(std::vector<const uri::Authority *> socket_configs)
{
	assert(socket_configs.size() > 0);

	struct addrinfo hints;
	int serv_sock;
	int status;

	std::vector<const uri::Authority *>::const_iterator it;
	for (it = socket_configs.begin(); it != socket_configs.end(); it++)
	{
		// for setsockopt()
		int yes = 1;
		struct addrinfo *res;
		int res_len = -1;

		// set socketaddress hints
		memset(&hints, 0, sizeof(hints));
		if ((*it)->family() == uri::Host::IPV4)
			hints.ai_family = AF_INET;
		else if ((*it)->family() == uri::Host::IPV6)
			hints.ai_family = AF_INET6;
		else
			hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		// testing the address and port and family
		//  std::cout << "host: " << (*it)->host.value << " port: " << (*it)->port << " family: " << (*it)->family() <<std::endl;
		if ((*it)->host.value == "::" || (*it)->host.value.empty() || (*it)->host.value == "0.0.0.0")
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
			// testing the address and port and family
			std::cout << "famliy: " << ai_ptr->ai_family << " socktype: " << ai_ptr->ai_socktype << " protocol: " << ai_ptr->ai_protocol << std::endl;
			// end of testing
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
#ifdef __linux__
			int yes = 1;
			if (ai_ptr->ai_family == AF_INET6 && setsockopt(serv_sock, IPPROTO_IPV6, IPV6_V6ONLY, &yes, sizeof(int)) == -1)
			{
				std::cerr << "setsockopt linux: " << strerror(errno) << std::endl;
				close(serv_sock);
				freeaddrinfo(res);
				return (kSetSockOptError);
			}
#endif
			if (fcntl(serv_sock, F_SETFL, O_NONBLOCK) == -1)
			{
				std::cerr << "fcntl: " << strerror(errno) << std::endl;
				close(serv_sock);
				freeaddrinfo(res);
				return (kFcntlError);
			}
			if (bind(serv_sock, ai_ptr->ai_addr, ai_ptr->ai_addrlen) == -1)
			{
				std::cerr << "bind of socket addresses: " << strerror(errno) << std::endl;
				close(serv_sock);
				continue;
			}
			// if (hints.ai_family == AF_UNSPEC && ai_ptr->ai_next != NULL)
			// {
			// 	printf("server socket is bind to both ipv4 and ipv6\n");
			// 	continue;
			// }
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
		// free the addrinfo struct and close the socket fd in SocketManager destructor
		ServerSocket server;
		server.socket = serv_sock;
		server.add_info = res;
		server.addr_to_bind = res_len;
		servers_.push_back(server);
		ws_database.register_server_socket(serv_sock, **it);
		// for testing the address and port
		void *addr;
		std::string ipver;
		char ipstr[INET6_ADDRSTRLEN];
		int port;
		// get the pointer to the address itself, different fields in IPv4 and IPv6:
		if (ai_ptr->ai_family == AF_INET)
		{ // IPv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)ai_ptr->ai_addr;
			addr = &(ipv4->sin_addr);
			port = ntohs(ipv4->sin_port);
			ipver = "IPv4";
		}
		else
		{ // IPv6, if ai_family is UNSPEC, getaddrinfo() will return socket addresses either IPv4 or IPv6. In out case, it returns IPv6.
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ai_ptr->ai_addr;
			addr = &(ipv6->sin6_addr);
			port = ntohs(ipv6->sin6_port);
			ipver = "IPv6";
		}
		// convert the IP to a string and print it:
		inet_ntop(ai_ptr->ai_family, addr, ipstr, sizeof ipstr);
		std::cout << "server socket: " << serv_sock << " is listeing on: " << ipver << ": [" << ipstr << "]:" << port << std::endl;
		// end of testing
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
			return (client_socket);
		}
	}
}

ssize_t SocketManager::recv_append(int client_socket, char *buf)
{
	ssize_t recv_len;
	recv_len = recv(client_socket, buf, BUF_SIZE, 0);
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
	}
	else
	{
		ClientSocket *client = get_one_client(client_socket);
		client->req_buf.append(buf, recv_len);
	}
	return (recv_len);
}

ssize_t SocketManager::send_to_client(int client_socket)
{
	ClientSocket *client = get_one_client(client_socket);
	ssize_t sent_bytes = 0;

	sent_bytes = send(client_socket, client->res_buf.c_str(), client->res_buf.size() > BUF_SIZE ? BUF_SIZE : client->res_buf.size(), 0);
	if (sent_bytes == -1)
	{
		std::cerr << "send: " << strerror(errno) << std::endl;
	}
	else
	{
		client->res_buf.erase(0, sent_bytes);
	}
	return (sent_bytes);
}

void SocketManager::delete_client_socket(int client_socket)
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

// getters
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

// udpate for managing client timeouts
// set first_recv_time to time of first recv of one request
void SocketManager::set_first_recv_time(int client_socket)
{
	ClientSocket *client = get_one_client(client_socket);
	if (client->first_recv_time.is_ok() == false)
	{
		client->first_recv_time = time(NULL); // set the time and set is_ok to true
	}
}

bool SocketManager::is_timeout(int client_socket)
{
	ClientSocket *client = get_one_client(client_socket);
	update_timeout(client);
	return (client->timeout);
}

void SocketManager::update_timeout(ClientSocket *client)
{
	// timeout when complete request not received within TIMEOUT
	if (client->first_recv_time.is_ok() == true)
	{
		time_t current_time = time(NULL);
		if (current_time - client->first_recv_time.value() > TIMEOUT * 1000)
		{
			client->timeout = true;
		}
	}
	// timeout when no new request received after last response sent
	else
	{
		time_t current_time = time(NULL);
		if (current_time - client->last_active > TIMEOUT * 1000)
		{
			client->timeout = true;
		}
	}
}

void SocketManager::set_time_assets(int client_socket)
{
	ClientSocket *client = get_one_client(client_socket);
	client->last_active = time(NULL);
	client->first_recv_time = Maybe<time_t>();
	client->timeout = false;
}
