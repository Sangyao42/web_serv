#pragma once

#include <arpa/inet.h>

typedef int SocketFd;

SocketFd  CreateSocket();

sockaddr_in CreateServerAddress(int port);

int ErrorMessage(const char* message);
