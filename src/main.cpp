#include "SocketManager.hpp"
#include "Configuration.hpp"
#include "SocketError.hpp"

#include <poll.h>

#include <vector>

namespace pollfds
{
	int add_server_fd(std::vector<struct pollfd> &pfds, std::vector<struct ServerSocket> &servers)
	{
		std::vector<struct ServerSocket>::iterator it;
		for (it = servers.begin(); it != servers.end(); it++)
		{
			struct pollfd pfd;
			pfd.fd = it->socket;
			pfd.events = POLLIN;
			pfds.push_back(pfd);
		}
		return (servers.size());
	}

	void add_client_fd(std::vector<struct pollfd> &pfds, int client_socket)
	{
		struct pollfd pfd;
		pfd.fd = client_socket;
		pfd.events = POLLIN | POLLOUT;
		pfds.push_back(pfd);
	}

	void delete_client_fd(std::vector<struct pollfd> &pfds, int i)
	{
		pfds.erase(pfds.begin() + i);
	}
}

int main(int argc, char **argv)
{
	int server_running = 1;
	Configuration conf;
	SocketManager sm;
	std::vector<struct pollfd> pfds;
	static char recv_buf[1024];

	int max_clients = conf.getMaxClients();
	int client_count = 0;
	sm.set_servers(conf.getPorts()); //assert inside make sure at least one server is available
	//vector of SocketConfiguration/ServerConfiguration
	int server_socket_count = pollfds::add_server_fd(pfds, sm.get_servers());
	int fd_count = server_socket_count;
	while (server_running)
	{
		//poll for events
		int poll_count = poll(pfds.data(), pfds.size(), -1);
		if (poll_count == -1)
		{
			std::cerr << "poll: " << strerror(errno) << std::endl;
			//TODO: error handling
			return (kPollError);
		}
		//check for events
		for (int i = 0; i < pfds.size(); i++)
		{
			if (pfds[i].revents & POLLIN)
			{
				if ( i < server_socket_count && client_count < max_clients)
				{
					//accept connection if max clients not reached
					int client_socket = sm.accept_client(sm.get_servers()[i].socket);
					if (client_socket == -1)
						continue ;
					else
					{
						//add client to pollfd
						pollfds::add_client_fd(pfds, client_socket);
						client_count++;
					}
				}
				else
				{
					//recv from client and add to request buffer
					ssize_t recv_len = sm.recv_append(pfds[i].fd, recv_buf);
					if (recv_len <= 0)
					{
						pollfds::delete_client_fd(pfds, i);
						client_count--;
					}
					else
					{
						//parse request
						//process request
					}
				}
			}
			else if (pfds[i].revents & POLLOUT)
			{
				//send response to client

			}
		}
	}
}
