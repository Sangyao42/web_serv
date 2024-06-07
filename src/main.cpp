#include "socket_manager/SocketManager.hpp"
#include "socket_manager/SocketError.hpp"
#include "Configuration.hpp"
#include "Client.hpp"

#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <cerrno>
#include <cassert>

#include <vector>

#define POLL_TIMEOUT 30

int server_running = 1;

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


void	SignalHandler(int signum)
{
	if (signum == SIGINT)
		server_running = 0;
}

int main(int argc, char **argv)
{
	(void) argc;
	(void) argv;
	// int server_running = 1;
	if (signal(SIGINT, SignalHandler) == SIG_ERR)
	{
		std::cerr << "signal: " << strerror(errno) << std::endl;
		return (1);
	}

	SocketManager sm;
	std::vector<struct Client> clients;
	std::vector<struct pollfd> pfds;

	// TEST: Configuration
	directive::MainBlock* main_block = new directive::MainBlock();
	Config2(*main_block);
	ws_database.set_main_block(main_block);
	// TEST: end

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
					int client_socket_fd = sm.accept_client(pfds[i].fd);
					if (client_socket_fd == -1)
						continue ;
					else
					{
						//add client to pollfd
						pollfds::AddClientFd(pfds, client_socket_fd);
						client_count++;
						//TODO: add client to clients vector ???
						struct Client client;
						struct ClientSocket *client_socket = sm.get_one_client(client_socket_fd);
						InitClient(&client, client_socket);
						clients.push_back(client);
					}
				}
			}
		}
		//check events for client sockets
		for (unsigned long i = server_socket_count; i < pfds.size(); i++)
		{
			//TODO: get the client struct by checking the pfds[i].fd
			struct Client *clt = GetClientBySocket(&clients, pfds[i].fd);
			//check if client socket is timeout
			bool timeout = sm.is_timeout(pfds[i].fd);
			if ((pfds[i].revents & POLLERR))
			{
				std::cout << "poll error" << std::endl;
				close(pfds[i].fd);
				//TODO: delete client to clients vector
				DeleteClientFromVector(&clients, pfds[i].fd);
				sm.delete_client_socket(pfds[i].fd);
				pollfds::DeleteClientFd(pfds, i);
				client_count--;
				continue;
			}
			else if ((pfds[i].revents & POLLHUP))
			{
				std::cout << "poll hup" << std::endl;
				close(pfds[i].fd);
				//TODO: delete client to clients vector
				DeleteClientFromVector(&clients, pfds[i].fd);
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
					//TODO: delete client to clients vector
					DeleteClientFromVector(&clients, pfds[i].fd);
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
					// set 408 in client struct and process::ProcessRequest()
					client_lifespan::UpdateStatusCode(clt, k408);
					std::cout << "timeout" << std::endl;
					pfds[i].events = POLLOUT;
					clt->keepAlive = false;
					process::ProcessRequest(clt);
					continue;
				}
				//recv from client and add to request buffer
				ssize_t recv_len = sm.recv_append(pfds[i].fd, recv_buf);
				//1st timestamp for timeout, using Maybe<time_t> first_recv_time in the else block of recv_append()
				if (recv_len <= 0)
				{
					std::cout << "recv_len <= 0" << std::endl;
					//TODO: delete client to clients vector
					DeleteClientFromVector(&clients, pfds[i].fd);
					close(pfds[i].fd);
					pollfds::DeleteClientFd(pfds, i);
					// sm.delete_client_socket(pfds[i].fd); Already done in recv_append()
					client_count--;
					continue;
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
					//parse request
					if (!clt->req->has_start_line())
					{
						std::pair<ParseMetaData, RequestLine> start_line = ParseRequestLine(clt->client_socket->req_buf);
						if (start_line.first.parse_error_flags != ParseError::kNone)
						{
							// handle error
						}
						else
						{
							clt->req->set_start_line(start_line.second);
							clt->client_socket->req_buf.erase(0, start_line.first.parse_length);
						}
					}
					if (!clt->req->has_headers())
					{
						std::pair<ParseMetaData, RequestLine> start_line = ParseRequestheaders(clt->client_socket->req_buf);
						if (start_line.first.parse_error_flags != ParseError::kNone)
						{
							// handle error
						}
						else
						{
							clt->req->set_headers(start_line.second);
							clt->client_socket->req_buf.erase(0, start_line.first.parse_length);
						}
					}
					CheckHeaderBeforeProcess(clt);
					if (req->is_chunked())
					{
						// find delimiter

						int	chunk_size = 0;
						bool	is_exceed_max_body_size = false;
						bool	is_continue_reading = false;
						do
						{
							std::pair<ParseMetaData, int> chunk_size_line = ParseChunkSizeLine(clt->client_socket->req_buf);
							if (chunk_size_line.first.parse_error_flags & kNoCRLFFound)
							{
								// continue reading
								break;
							}
							else if (chunk_size_line.first.parse_error_flags & kSyntaxError)
							{
								// handle error
								break;
							}
							else
							{
								chunk_size = chunk_size_line.second;
								clt->client_socket->req_buf.erase(0, chunk_size_line.first.parse_length);
							}
							if ((clt->req.body.size() + chunk_size) > ws_database.max_body_size())
							{
								is_exceed_max_body_size = true;
							}
							ParseMetaData = ParseChunkData(clt->client_socket->req_buf, chunk_size);
							if (ParseMetaData.parse_error_flags & kNoCRLFFound)
							{
								// continue reading
								break;
							}
							else if (ParseMetaData.parse_error_flags & kSyntaxError)
							{
								// handle error
								break;
							}
							std::string chunk = clt->client_socket->req_buf.substr(0, chunk_size);
							if (!is_exceed_max_body_size)
							{
								ctl->req.body.append(chunk);
							}
							clt->client_socket->req_buf.erase(0, chunk_size);
						} while (chunk_size > 0);
						IgnoreEntityHeaders(clt->client_socket->req_buf);
						if (is_continue_reading)
						{}
					}
					else
					{
						//if request.content-length exceeds max body size
							//set pfds[i].events = POLLOUT;
							//generate 413 Request Entity Too Large and build std::string response in client
							//set 413 in client struct and process::ProcessRequest()
							// continue;
						HeaderInt *content_length = dynamic_cast<HeaderInt *> (clt->req->returnValueAsPointer("Content-Length"));
						if (content_length && content_length.content() > )
						//if request is complete (req.size() - parsed_length>= content-length)
							//set pfds[i].events = POLLOUT;
							// process request
							// continue;
					}
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
					//TODO: delete client from clients vector
					DeleteClientFromVector(&clients, pfds[i].fd);
					pollfds::DeleteClientFd(pfds, i);
					sm.delete_client_socket(pfds[i].fd);
					client_count--;
				}
				else
				{
					// TODO :if keeplive is false
					// 				close(pfds[i].fd);
					// sm.delete_client_socket(pfds[i].fd);
					// pollfds::DeleteClientFd(pfds, i);
					// client_count--;
					//TODO: delete client from clients vector
					if (IsClientAlive(clt) == false)
					{
						close(pfds[i].fd);
						DeleteClientFromVector(&clients, pfds[i].fd);
						sm.delete_client_socket(pfds[i].fd);
						pollfds::DeleteClientFd(pfds, i);
						client_count--;
					}
					else
					{
						//set pfds[i].events = POLLIN;
						pfds[i].events = POLLIN;
						//set last_active time to time(NULL);
						//set first_recv_time to to Maybe<time_t> init_time, which is_ok_ = false; and value does not matter
						//set timeout to false
						sm.set_time_assets(pfds[i].fd);
						//TODO: reset Client struct for new request
						ResetClient(&clients);
					}
				}
			}
		}
	}
	//TODO: cleanup: close all sockets, clean vector of pollfds

}
