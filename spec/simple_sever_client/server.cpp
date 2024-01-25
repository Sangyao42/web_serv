#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#include "common.hpp"

int BindSocket(SocketFd serverSocket, const sockaddr_in& serverAddress)
{
  return bind(serverSocket, \
               reinterpret_cast<const struct sockaddr*>(&serverAddress), \
               sizeof(serverAddress));
}

SocketFd  AcceptClientConnection(SocketFd serverSocket, sockaddr_in& clientAddress)
{
  socklen_t clientAddressLength = sizeof(clientAddress);
  return accept(serverSocket, \
                reinterpret_cast<struct sockaddr*>(&clientAddress), \
                &clientAddressLength);
}

void  SendResponseToClient(SocketFd clientSocket)
{
  char buffer[1024];
  ssize_t bytesRead;
  std::string client_message;

  // Read data from the client
  while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
    // Process the received data (in this example, just print it)
    client_message = std::string(buffer, bytesRead);
    client_message[bytesRead] = '\0';
    std::cout << "Received: " << client_message << std::endl;

    // Send a response back to the client
    send(clientSocket, client_message.c_str(), strlen(client_message.c_str()), 0);
  }
}

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
    return EXIT_FAILURE;
  }
  const int port = std::stoi(argv[1]);
  // Create a socket
  const SocketFd serverSocket = CreateSocket();
  if (serverSocket == -1)
    return ErrorMessage("Error creating socket");

  // Bind the socket to a specific address and port
  if (BindSocket(serverSocket, CreateServerAddress(port)) == -1)
  {
    close(serverSocket);
    return ErrorMessage("Error binding socket");
  }

  // Listen for incoming connections
  if (listen(serverSocket, 10) == -1)
  {
    close(serverSocket);
    return ErrorMessage("Error listening on socket");
  }

  std::cout << "Server listening on port 12345" << std::endl;

  // Accept connections and handle them
  sockaddr_in clientAddress;
  while (true)
  {
    const int clientSocket = AcceptClientConnection(serverSocket, clientAddress);
    if (clientSocket == -1) {
        std::cerr << "Error accepting connection" << std::endl;
        continue;
    }
    SendResponseToClient(clientSocket);
    // Close the client socket
    close(clientSocket);
  }

  // Close the server socket (this part will not be reached in this simple example)
  close(serverSocket);

  return 0;
}

