#include "socket_manager/SocketManager.hpp"
#include "socket_manager/SocketError.hpp"
#include "Configuration.hpp"

#include <poll.h>
#include <unistd.h>

#include <vector>

#define POLL_TIMEOUT 30

namespace pollfds
{
	int add_server_fd(std::vector<struct pollfd> &pfds, std::vector<struct ServerSocket> servers)
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
		pfd.events = POLLIN;
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
	SocketManager sm;
	std::vector<struct pollfd> pfds;
	static char recv_buf[1024];

	int max_clients = ws_database.worker_connections();
	int client_count = 0;
	int server_socket_count = pollfds::add_server_fd(pfds, sm.get_servers());
	while (server_running)
	{
		//poll for events
		int poll_count = poll(pfds.data(), pfds.size(), POLL_TIMEOUT * 1000);
		if (poll_count == -1)
		{
			std::cerr << "poll: " << strerror(errno) << std::endl;
			//TODO: error handling
			return (kPollError);
		}
		//check for events
		// for (int i = 0; i < pfds.size(); i++)
		// {
		// 	if (pfds[i].revents & POLLIN)
		// 	{
		// 		if ( i < server_socket_count && client_count < max_clients)
		// 		{
		// 			//accept connection if max clients not reached
		// 			int client_socket = sm.accept_client(pfds[i].fd);
		// 			if (client_socket == -1)
		// 				continue ;
		// 			else
		// 			{
		// 				//add client to pollfd
		// 				pollfds::add_client_fd(pfds, client_socket);
		// 				client_count++;
		// 			}
		// 		}
		// 		else
		// 		{
		// 			//check request timeout before recv and set the bool of timeout to false and continue???
		// 			//recv from client and add to request buffer
		// 			ssize_t recv_len = sm.recv_append(pfds[i].fd, recv_buf);
		// 			//1st timestamp for timeout, using Maybe<time_t> init_time;
		// 			if (recv_len <= 0)
		// 			{
		// 				close(pfds[i].fd);
		// 				pollfds::delete_client_fd(pfds, i);
		// 				client_count--;
		// 			}
		// 			else
		// 			{
		// 				//parse request
		// 				//process request
		// 			}
		// 		}
		// 	}
		// 	else if (pfds[i].revents & POLLOUT)
		// 	{
		// 		//send response to client
		// 		ssize_t send_len = sm.send_all(pfds[i].fd);
		// 		//TODO: do I need to close the client socket if send fails?
		// 		if (send_len == -1)
		// 		{
		// 			close(pfds[i].fd);
		// 			pollfds::delete_client_fd(pfds, i);
		// 			client_count--;
		// 		}
		// 	}
		// }

		//check events for server sockets
		for (int i = 0; i < server_socket_count; i++)
		{
			if (pfds[i].revents & POLLIN)
			{
				if (client_count < max_clients)
				{
					//accept connection if max clients not reached
					int client_socket = sm.accept_client(pfds[i].fd);
					if (client_socket == -1)
						continue ;
					else
					{
						//add client to pollfd
						pollfds::add_client_fd(pfds, client_socket);
						client_count++;
					}
				}
			}
		}
		//check events for client sockets
		for (int i = server_socket_count; i < pfds.size(); i++)
		{
			//check if client socket is timeout
			bool timeout = sm.is_timeout(pfds[i].fd);
			if ((pfds[i].revents & (POLLIN | POLLOUT)) == 0) //no client events
			{
				if (timeout == true)
				{
					close(pfds[i].fd);
					pollfds::delete_client_fd(pfds, i);
					client_count--;
				}
				continue;
			}
			//socket is ready for reading
			if (pfds[i].revents & POLLIN)
			{
				if (timeout == true)
				{
					//generate 408 Request Timeout and build std::string response in client
					pfds[i].events = POLLOUT;
					continue;
				}
				//recv from client and add to request buffer
				ssize_t recv_len = sm.recv_append(pfds[i].fd, recv_buf);
				//1st timestamp for timeout, using Maybe<time_t> first_recv_time in the else block of recv_append()
				if (recv_len <= 0)
				{
					close(pfds[i].fd);
					pollfds::delete_client_fd(pfds, i);
					client_count--;
				}
				else
				{
					sm.set_first_recv_time(pfds[i].fd); //set first recv time if it is not set
					//if request exceeds max body size
						//set pfds[i].events = POLLOUT;
						//generate 413 Request Entity Too Large and build std::string response in client
					//if request is complete
						//set pfds[i].events = POLLOUT;
						//parse request
						//process request
				}
			}
			//socket is ready for writing
			else if (pfds[i].revents & POLLOUT)
			{
				//send response to client
				ssize_t send_len = sm.send_all(pfds[i].fd);
				if (send_len == -1)
				{
					close(pfds[i].fd);
					pollfds::delete_client_fd(pfds, i);
					client_count--;
				}
				else
				{
					//set pfds[i].events = POLLIN;
					//set last_active time to time(NULL);
					//set first_recv_time to to Maybe<time_t> init_time, which is_ok_ = false; and value does not matter
					//set timeout to false
					pfds[i].events = POLLIN;
					sm.set_time_assets(pfds[i].fd);
				}
			}
		}
	}
}
