#pragma once

#include <vector>

#include "Configuration/Directive/Socket.hpp"
#include "Configuration/Directive/Block/Server.hpp"

namespace cache
{
  struct ServerQuery
  {
    int                                         server_socket_fd;
    const directive::Socket*                    socket;
    std::vector<const directive::ServerBlock*>  server_blocks;

    ServerQuery();
    ServerQuery(const directive::Socket* socket, const directive::ServerBlock* server_block);
  };
} // namespace cache
