#include "common.hpp"

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>

SocketFd  CreateSocket()
{
  return socket(AF_INET, SOCK_STREAM, 0);
}

sockaddr_in CreateServerAddress(int port)
{
  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);  // Use the same port as the server
  inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);
  // serverAddress.sin_addr.s_addr = INADDR_ANY; // alternative
  return serverAddress;
}

int ErrorMessage(const char* message)
{
  std::cerr << message << std::endl;
  return EXIT_FAILURE;
}
