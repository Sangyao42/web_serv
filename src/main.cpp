#include "socket_manager/SocketManager.hpp"
#include "socket_manager/SocketError.hpp"
#include "Configuration.hpp"

#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <cerrno>
#include <cassert>

#include <vector>

#define POLL_TIMEOUT 30

// TODO: Add info to Client struct

void  Config2(directive::MainBlock& main);

namespace pollfds
{
	int AddServerFd(std::vector<struct pollfd> &pfds, std::vector<struct ServerSocket> servers)
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

	void AddClientFd(std::vector<struct pollfd> &pfds, int client_socket)
	{
		struct pollfd pfd;
		pfd.fd = client_socket;
		pfd.events = POLLIN;
		pfds.push_back(pfd);
	}

	void DeleteClientFd(std::vector<struct pollfd> &pfds, int i)
	{
		pfds.erase(pfds.begin() + i);
	}
}

int main(int argc, char **argv)
{
	(void) argc;
	(void) argv;
	int server_running = 1;
	SocketManager sm;
	std::vector<struct pollfd> pfds;

	directive::MainBlock* main_block = new directive::MainBlock();
	Config2(*main_block);
	ws_database.set_main_block(main_block);

	static char recv_buf[RECV_BUF_SIZE];

	int max_clients = ws_database.worker_connections();
	int client_count = 0;
	enum SocketError err = sm.set_servers(ws_database.all_server_sockets());
	if (err != kNoError)
		return (err);
	int server_socket_count = pollfds::AddServerFd(pfds, sm.get_servers());
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
						pollfds::AddClientFd(pfds, client_socket);
						client_count++;
					}
				}
			}
		}
		//check events for client sockets
		for (unsigned long i = server_socket_count; i < pfds.size(); i++)
		{
			//check if client socket is timeout
			bool timeout = sm.is_timeout(pfds[i].fd);
			if ((pfds[i].revents & POLLERR))
			{
				std::cout << "poll error" << std::endl;
				close(pfds[i].fd);
				sm.delete_client_socket(pfds[i].fd);
				pollfds::DeleteClientFd(pfds, i);
				client_count--;
				continue;
			}
			else if ((pfds[i].revents & POLLHUP))
			{
				std::cout << "poll hup" << std::endl;
				close(pfds[i].fd);
				sm.delete_client_socket(pfds[i].fd);
				pollfds::DeleteClientFd(pfds, i);
				client_count--;
				continue;
			}
			else if ((pfds[i].revents & (POLLIN | POLLOUT)) == 0) //no client events
			{
				if (timeout == true)
				{
					std::cout << "timeout" << std::endl;
					close(pfds[i].fd);
					sm.delete_client_socket(pfds[i].fd);
					pollfds::DeleteClientFd(pfds, i);
					client_count--;
				}
				continue;
			}
			//socket is ready for reading
			else if (pfds[i].revents & POLLIN)
			{
				if (timeout == true)
				{
					//TODO: generate 408 Request Timeout and build std::string response in client
					std::cout << "timeout" << std::endl;
					pfds[i].events = POLLOUT;
					continue;
				}
				//recv from client and add to request buffer
				ssize_t recv_len = sm.recv_append(pfds[i].fd, recv_buf);
				//1st timestamp for timeout, using Maybe<time_t> first_recv_time in the else block of recv_append()
				if (recv_len <= 0)
				{
					std::cout << "recv_len <= 0" << std::endl;
					close(pfds[i].fd);
					pollfds::DeleteClientFd(pfds, i);
					// sm.delete_client_socket(pfds[i].fd);
					client_count--;
				}
				else
				{
					//TEST: the request buffer
					std::string whatigot = sm.get_one_client(pfds[i].fd)->req_buf;
					std::cout << "client request: " << std::endl;
					std::cout << whatigot << std::endl;
					std::cout << "recv_len: " << recv_len << std::endl;
					unsigned long pos = whatigot.find("\r\n\r\n");
					if (pos != std::string::npos && pos + 4 == whatigot.size())
						std::cout << "end of request" << std::endl;
					else
						std::cout << "not end of request" << std::endl;
					//TEST: end
					sm.set_first_recv_time(pfds[i].fd); //set first recv time if it is not set
					//TODO:
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
				std::cout << "POLLOUT" << std::endl;
				//send response to client
				ssize_t send_len = sm.send_all(pfds[i].fd);
				if (send_len == -1)
				{
					close(pfds[i].fd);
					pollfds::DeleteClientFd(pfds, i);
					sm.delete_client_socket(pfds[i].fd);
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
