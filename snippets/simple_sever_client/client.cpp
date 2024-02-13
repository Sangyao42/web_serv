#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

#include "common.hpp"

int  ConnectToServer(SocketFd clientSocket, const sockaddr_in& serverAddress)
{
  return connect(clientSocket, \
                 reinterpret_cast<const struct sockaddr*>(&serverAddress), \
                 sizeof(serverAddress));
}

void  ReceiveResponseFromServer(SocketFd clientSocket)
{
  char buffer[1024];
  const ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
  if (bytesRead > 0) {
    std::cout << "Server response: " << std::string(buffer, bytesRead) << std::endl;
  } else {
    std::cerr << "Error receiving data from server" << std::endl;
  }
}

int main(int argc, char** argv)
{
  if (argc != 3)
  {
    std::cerr << "Usage: " << argv[0] << " <port> <message>" << std::endl;
    return EXIT_FAILURE;
  }
  const int port = std::atoi(argv[1]);
  const char* message = argv[2];

  if (strlen(message) > 1024)
    return ErrorMessage("Message too long");
  else if (strlen(message) == 0)
    return ErrorMessage("Message too short");

  // Create a socket
  const SocketFd client_socket = CreateSocket();
  if (client_socket == -1)
    return ErrorMessage("Error creating socket");

  // Specify the server address and port to connect to and
  // connect to the server
  if (ConnectToServer(client_socket, CreateServerAddress(port)) == -1)
  {
    close(client_socket);
    return ErrorMessage("Error connecting to server");
  }
  // Send data to the server
  send(client_socket, message, strlen(message), 0);
  // Receive and print the response from the server
  ReceiveResponseFromServer(client_socket);
  // Close the client socket
  close(client_socket);

  return EXIT_SUCCESS;
}

