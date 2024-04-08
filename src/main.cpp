#include "SocketManager.hpp"
#include "Configuration.hpp"
#include "SocketError.hpp"

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
			if (poll_count == 0)
			{
			//TODO: do I need to handle timeout here to close the client whose request is timeout?
			}
			//check if client socket is timeout
			bool timeout = sm.is_timeout(pfds[i].fd);
			//socket is ready for reading
			if (timeout == false && pfds[i].revents & POLLIN)
			{
				//check request timeout before recv and set the bool of timeout to false and continue???
				//recv from client and add to request buffer
				ssize_t recv_len = sm.recv_append(pfds[i].fd, recv_buf);
				//1st timestamp for timeout, using Maybe<time_t> init_time in the else block of recv_append()
				if (recv_len <= 0)
				{
					close(pfds[i].fd);
					pollfds::delete_client_fd(pfds, i);
					client_count--;
				}
				else
				{
					sm.update_init_time(pfds[i].fd);
					//parse request
					//process request
				}
			}
			//socket is ready for writing
			else if (pfds[i].revents & POLLOUT)
			{
				if (timeout == true)
				{
					//generate 408 Request Timeout and build std::string response in client
				}
				//send response to client
				ssize_t send_len = sm.send_all(pfds[i].fd);
				if (send_len == -1)
				{
					close(pfds[i].fd);
					pollfds::delete_client_fd(pfds, i);
					client_count--;
				}
			}
		}
	}
}

/**
 * request timeout:
 * When the 408 Request Timeout error message is received,
 * it means that a client has initiated a request but for some reason,
 * it has not been transmitted in full. This may occur because an internet connection is very slow, or has been dropped.
 * The response will include the Connection header, specifying that it has been closed.
 *
*/

/**
 * TODO:
 * 1. refactory: related to  configuration class
 *
 * 2. refactory: related to request timeout --
 * 2.1 using Maybe<time_t> init_time; -- done
 * 2.2 check request timeout before recv and set the bool of timeout
 * 2.3 seperate for loop for server sockets and client sockets -- done
 *
 * 3. fcntl() for both server and client sockets -- done
 *
 * 4. check send_all function for checking if send() fails -- done
*/