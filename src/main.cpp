#include "socket_manager/SocketManager.hpp"
#include "socket_manager/SocketError.hpp"
#include "Configuration.hpp"
#include "Client.hpp"
#include "Http/Parser.hpp"
#include "Configuration/Parser.hpp"

#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <cerrno>
#include <cassert>
#include <signal.h>

#include <vector>

#define POLL_TIMEOUT 30

int server_running = 1;

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

void PrintDebugMessage(const char *message, int fd)
{
#ifndef NDEBUG
	std::cerr << "Client " << fd << ": " << message << std::endl;
#else
	(void)message;
	(void)fd;
#endif
}

void PrintClients(std::vector<struct Client> &clients)
{
#ifndef NDEBUG
	std::cerr << "size " << clients.size() << " : ";
	for (std::vector<struct Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		std::cerr << it->client_socket->socket << ", ";
	}
	std::cerr << std::endl;
#else
	(void)clients;
#endif
}

void DeleteClient(std::vector<struct Client> &clients, SocketManager &sm, int client_fd)
{
	int index = client_lifespan::DeleteClientFromVector(clients, client_fd);
	sm.delete_client_socket(client_fd);
	for (std::vector<struct Client>::iterator it = clients.begin() + index; it != clients.end(); it++)
	{
		it->client_socket--;
	}
}

void SignalHandler(int signum)
{
	if (signum == SIGINT)
		server_running = 0;
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " configuration_file" << std::endl;
		return (1);
	}
	else if (signal(SIGINT, SignalHandler) == SIG_ERR)
	{
		std::cerr << "signal: " << strerror(errno) << std::endl;
		return (1);
	}
	{
		std::ifstream file(argv[1], std::ios::in | std::ios::ate);
		if (!file.is_open())
		{
			std::cerr << "Unable to open file " << argv[1] << std::endl;
			return (1);
		}
		size_t size = file.tellg();
		char *string = new char[size];
		file.seekg(0, std::ios::beg);
		file.read(string, size);
		file.close();

		directive_parser::ParseOutput parsed_main_block = directive_parser::ParseMainBlock(directive_parser::ParseInput(string, size));
		delete[] string;
		if (parsed_main_block.is_valid())
		{
			directive::MainBlock *main_block = static_cast<directive::MainBlock *>(parsed_main_block.result);
			ws_database.set_main_block(main_block);
		}
		else
		{
			std::cerr << "Configuration file parsing error" << std::endl;
			return (1);
		}
	}

	std::vector<struct Client> clients;
	std::vector<struct pollfd> pfds;
	static char recv_buf[RECV_BUF_SIZE];

	int max_clients = ws_database.worker_connections();
	SocketManager sm(max_clients);
	int client_count = 0;
	SocketError err;
	{
		std::vector<const uri::Authority *> sockets = ws_database.all_server_sockets();
		clients.reserve(max_clients);
		pfds.reserve(max_clients + sockets.size());
		err = sm.set_servers(sockets);
	}
	if (err != kNoError)
		return (err);
	int server_socket_count = pollfds::AddServerFd(pfds, sm.get_servers());
	while (server_running)
	{
		// poll for events
		int poll_count = poll(pfds.data(), pfds.size(), POLL_TIMEOUT * 1000);
		if (poll_count == -1)
		{
			std::cerr << "poll: " << strerror(errno) << std::endl;
			// TODO: error handling
			err = kPollError;
			break;
		}
		// check events for server sockets
		for (int i = 0; i < server_socket_count; i++)
		{
			if (pfds[i].revents & POLLIN)
			{
				if (client_count < max_clients)
				{
					// accept connection if max clients not reached
					int client_socket_fd = sm.accept_client(pfds[i].fd);
					if (client_socket_fd == -1)
						continue;
					else
					{
						// add client to pollfd
						pollfds::AddClientFd(pfds, client_socket_fd);
						client_count++;
						// add client to clients vector
						struct Client client;
						struct ClientSocket *client_socket = sm.get_one_client(client_socket_fd);
						client_lifespan::InitClient(client, client_socket);
						clients.push_back(client);
#ifndef NDEBUG
						std::cerr << "Client " << client.client_socket->socket << ": accepted at server fd " << pfds[i].fd << std::endl;
#endif
						PrintClients(clients);
					}
				}
			}
		}
		// check events for client sockets
		for (unsigned long i = server_socket_count; i < pfds.size(); i++)
		{
			// get the client struct by checking the pfds[i].fd
			struct Client *clt = client_lifespan::GetClientByFd(clients, pfds[i].fd);
			// check if client socket is timeout
			bool timeout = sm.is_timeout(pfds[i].fd);
			if ((pfds[i].revents & POLLERR))
			{
				PrintDebugMessage("POLLERR (removed from pfds)", pfds[i].fd);
				close(pfds[i].fd);
				DeleteClient(clients, sm, pfds[i].fd);
				pollfds::DeleteClientFd(pfds, i);
				PrintClients(clients);
				client_count--;
				i--;
				continue;
			}
			else if ((pfds[i].revents & POLLHUP))
			{
				PrintDebugMessage("POLLHUP (removed from pfds)", pfds[i].fd);
				close(pfds[i].fd);
				DeleteClient(clients, sm, pfds[i].fd);
				pollfds::DeleteClientFd(pfds, i);
				PrintClients(clients);
				client_count--;
				i--;
				continue;
			}
			else if ((pfds[i].revents & (POLLIN | POLLOUT)) == 0) // no client events
			{
				if (timeout == true)
				{
					PrintDebugMessage("Timeout at revents that are not POLLIN nor POLLOUT (removed from pfds)", pfds[i].fd);
					close(pfds[i].fd);
					DeleteClient(clients, sm, pfds[i].fd);
					pollfds::DeleteClientFd(pfds, i);
					PrintClients(clients);
					client_count--;
					i--;
				}
				continue;
			}
			// socket is ready for reading
			else if (pfds[i].revents & POLLIN)
			{
				if (timeout == true)
				{
					PrintDebugMessage("Timeout at POLLIN", pfds[i].fd);
					// generate 408 Request Timeout and build std::string response in client
					// set 408 in client struct and process::ProcessRequest()
					// client_lifespan::UpdateStatusCode(clt, k408);
					clt->status_code = k408;
					pfds[i].events = POLLOUT;
					clt->keepAlive = false;
					process::ProcessRequest(clt);
					continue;
				}
				// recv from client and add to request buffer
				ssize_t recv_len = sm.recv_append(pfds[i].fd, recv_buf);
				// 1st timestamp for timeout, using Maybe<time_t> first_recv_time in the else block of recv_append()
				if (recv_len <= 0)
				{
					PrintDebugMessage("recv_len <= 0 (removed from pfds)", pfds[i].fd);
					close(pfds[i].fd);
					DeleteClient(clients, sm, pfds[i].fd);
					pollfds::DeleteClientFd(pfds, i);
					PrintClients(clients);
					client_count--;
					i--;
					continue;
				}
				else
				{
					// TEST: the request buffer
					//  std::string whatigot = sm.get_one_client(pfds[i].fd)->req_buf;
					//  std::cout << "client request: " << std::endl;
					//  std::cout << whatigot << std::endl;
					//  std::cout << "recv_len: " << recv_len << std::endl;
					//  // unsigned long pos = whatigot.find("\r\n\r\n");
					//  if (pos != std::string::npos && pos + 4 == whatigot.size())
					//  	std::cout << "end of request(only headers)" << std::endl;
					//  else
					//  {
					//  	unsigned long pos = whatigot.find_last_of("\r\n");
					//  	if (pos != std::string::npos && pos + 1 == whatigot.size())
					//  		std::cout << "end of request(with body)" << std::endl;
					//  	else
					//  		std::cout << "not end of request" << std::endl;
					//  }
					sm.set_first_recv_time(pfds[i].fd); // set first recv time if it is not set
					if (!clt->continue_reading)
					{
						PrintDebugMessage("POLLIN request start", pfds[i].fd);
						PrintClients(clients);
						enum ParseError error = kNone;
						size_t find_index = clt->client_socket->req_buf.find("\r\n");
						if (find_index != std::string::npos)
						{
							temporary::arena.clear();
							http_parser::ParseOutput parsed_request_line = http_parser::ParseRequestLine(http_parser::StringSlice(clt->client_socket->req_buf.c_str(), find_index + 2));
							if (!parsed_request_line.is_valid())
							{
								// handle only syntax error
								PrintDebugMessage("Request line syntax error", pfds[i].fd);
								clt->status_code = k400;
								clt->keepAlive = false;
								process::ProcessRequest(clt);
								pfds[i].events = POLLOUT;
								temporary::arena.clear();
								continue;
							}
							else
							{
								RequestLine request_line;
								error = AnalysisRequestLine(static_cast<http_parser::PTNodeRequestLine *>(parsed_request_line.result), &request_line);
								if (error != kNone)
									clt->consume_body = false;
								clt->status_code = ParseErrorToStatusCode(error);
								clt->req.setRequestLine(request_line);
								clt->client_socket->req_buf.erase(0, parsed_request_line.length + 2);
							}
							temporary::arena.clear();
						}
						find_index = clt->client_socket->req_buf.find("\r\n\r\n");
						if (find_index != std::string::npos)
						{
							temporary::arena.clear();
							http_parser::ParseOutput parsed_headers = ParseFields(http_parser::StringSlice(clt->client_socket->req_buf.c_str(), find_index + 4));
							if (!parsed_headers.is_valid())
							{
								// handle only syntax error
								PrintDebugMessage("Request fields syntax error", pfds[i].fd);
								clt->status_code = k400;
								clt->keepAlive = false;
								process::ProcessRequest(clt);
								pfds[i].events = POLLOUT;
								temporary::arena.clear();
								continue;
							}
							else
							{
								enum ParseError errorReqHeaders = AnalysisRequestHeaders(static_cast<http_parser::PTNodeFields *>(parsed_headers.result), &clt->req.headers_);
								if (error == kSyntaxError)
								{
									// handle only syntax error
									PrintDebugMessage("Request field value syntax error", pfds[i].fd);
									clt->status_code = k400;
									clt->keepAlive = false;
									process::ProcessRequest(clt);
									pfds[i].events = POLLOUT;
									continue;
								}
								if (errorReqHeaders != kNone)
									clt->consume_body = false;
								if (error == kNone)
									error = errorReqHeaders;
								clt->status_code = ParseErrorToStatusCode(error);
								clt->client_socket->req_buf.erase(0, parsed_headers.length + 2);
							}
							temporary::arena.clear();
						}
						client_lifespan::CheckHeaderBeforeProcess(clt); // We Suppose the first read will contain all the headers
					}
					if (clt->is_chunked)
					{
						// find delimiter
						// int	chunk_size = 0;
						// clt->continue_reading = false;
						// bool syntax_error_during_unchunk = false;
						// do
						// {
						// 	std::pair<ParseMetaData, int> chunk_size_line = ParseChunkSizeLine(clt->client_socket->req_buf);
						// 	if (chunk_size_line.first.parse_error_flags == kNoCRLFFound)
						// 	{
						// 		clt->continue_reading = true;
						// 		break;
						// 	}
						// 	else if (chunk_size_line.first.parse_error_flags == kSyntaxError)
						// 	{
						// 		// handle syntax error
						// 		syntax_error_during_unchunk = true;
						// 		break;
						// 	}
						// 	else
						// 	{
						// 		chunk_size = chunk_size_line.second;
						// 		clt->client_socket->req_buf.erase(0, chunk_size_line.first.parse_length);
						// 	}
						// 	if ((clt->req.requestBody_.size() + chunk_size) > clt->max_body_size)
						// 	{
						// 		clt->exceed_max_body_size = true;
						// 	}
						// 	ParseMetaData = ParseChunkData(clt->client_socket->req_buf, chunk_size);
						// 	if (ParseMetaData.parse_error_flags == kNoCRLFFound)
						// 	{
						// 		clt->continue_reading = true;
						// 		break;
						// 	}
						// 	else if (ParseMetaData.parse_error_flags ==  kSyntaxError)
						// 	{
						// 		// handle syntax error
						// 		syntax_error_during_unchunk = true;
						// 		break;
						// 	}
						// 	std::string chunk = clt->client_socket->req_buf.substr(0, chunk_size);
						// 	if (!clt->exceed_max_body_size)
						// 	{
						// 		if (clt->consume_body)
						// 			clt->req.requestBody_.append(chunk);
						// 	}
						// 	clt->client_socket->req_buf.erase(0, chunk_size);
						// } while (chunk_size > 0);
						// if (syntax_error_during_unchunk) // when syntax error happens
						// {
						// 	clt->status_code = k400;
						// 	clt->keepAlive = false;
						// 	process::ProcessRequest(clt);
						// 	pfds[i].events = POLLOUT;
						// 	continue;
						// }
						// IgnoreEntityHeaders(clt->client_socket->req_buf);
						// if (!CheckCrlf(clt->client_socket->req_buf))
						// {
						// 	clt->status_code = k400;
						// 	clt->keepAlive = false;
						// 	process::ProcessRequest(clt);
						// 	pfds[i].events = POLLOUT;
						// 	continue;
						// }
						// clt->client_socket->req_buf.erase(0, 2);
						// if (clt->continue_reading)
						// 	continue;
						// if (clt->exceed_max_body_size)
						// {
						// 	clt->status_code = k413;
						// 	clt->keepAlive = false;
						// }
						// process::ProcessRequest(clt);
						// pfds[i].events = POLLOUT;
					}
					else
					{
						if (!clt->continue_reading)
						{
							HeaderInt *content_length = static_cast<HeaderInt *>(clt->req.returnValueAsPointer("Content-Length"));
							if (content_length && content_length->content() > (int)clt->max_body_size)
							{
								PrintDebugMessage("Exceed max body size", pfds[i].fd);
								clt->exceed_max_body_size = true;
								clt->status_code = k413;
								clt->keepAlive = false;
								process::ProcessRequest(clt);
								pfds[i].events = POLLOUT;
								continue;
							}
							if (content_length)
								clt->content_length = content_length->content();
						}
						clt->continue_reading = false;
						if (clt->content_length > clt->client_socket->req_buf.length())
						{
							clt->continue_reading = true;
							continue;
						}
						if (clt->consume_body)
						{
							clt->req.requestBody_ = clt->client_socket->req_buf.substr(0, clt->content_length);
						}
						clt->client_socket->req_buf.erase(0, clt->content_length);
						process::ProcessRequest(clt);
						pfds[i].events = POLLOUT;
					}
				}
				PrintDebugMessage("POLLIN request end", pfds[i].fd);
			}
			// socket is ready for writing
			else if (pfds[i].revents & POLLOUT)
			{
				// send response to client
				PrintDebugMessage("POLLOUT", pfds[i].fd);
				PrintClients(clients);
				ssize_t send_len = sm.send_all(pfds[i].fd);
				if (send_len == -1)
				{
					PrintDebugMessage("POLLOUT send() error (removed from pdfs)", pfds[i].fd);
					close(pfds[i].fd);
					DeleteClient(clients, sm, pfds[i].fd);
					pollfds::DeleteClientFd(pfds, i);
					PrintClients(clients);
					client_count--;
					i--;
				}
				else
				{
					// if keeplive is false
					if (client_lifespan::IsClientAlive(clt) == false)
					{
						PrintDebugMessage("POLLOUT is not alive (removed from pdfs)", pfds[i].fd);
						close(pfds[i].fd);
						DeleteClient(clients, sm, pfds[i].fd);
						pollfds::DeleteClientFd(pfds, i);
						PrintClients(clients);
						client_count--;
						i--;
					}
					else
					{
						// set pfds[i].events = POLLIN;
						pfds[i].events = POLLIN;
						// set last_active time to time(NULL);
						// set first_recv_time to to Maybe<time_t> init_time, which is_ok_ = false; and value does not matter
						// set timeout to false
						sm.set_time_assets(pfds[i].fd);
						// reset Client struct for new request
						client_lifespan::ResetClient(*clt);
						PrintDebugMessage("Reset", pfds[i].fd);
						PrintClients(clients);
					}
				}
			}
		}
	}
	// cleanup: close all client sockets, clean vector of pollfds
	for (unsigned long i = server_socket_count; i < pfds.size(); i++)
	{
		close(pfds[i].fd);
	}
	std::cout << "server stopped" << std::endl;
	return (err);
}
