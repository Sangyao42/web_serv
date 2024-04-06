#pragma once

#include <vector>

#include "Uri/Authority.hpp"
#include "Configuration/Directive/Block/Server.hpp"

namespace cache
{
  struct ServerQuery
  {
    int                                         server_socket_fd;
    const uri::Authority*                       socket;
    std::vector<const directive::ServerBlock*>  server_blocks;

    ServerQuery();
    ServerQuery(const uri::Authority* socket, const directive::ServerBlock* server_block);
  };
} // namespace cache
