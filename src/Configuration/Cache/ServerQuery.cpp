#include "ServerQuery.hpp"

namespace cache
{
  ServerQuery::ServerQuery()
    : server_socket_fd(-1),
      socket(NULL),
      server_blocks() {}

  ServerQuery::ServerQuery(const directive::Socket* socket,
                                    const directive::ServerBlock* server_block)
    : server_socket_fd(-1),
      socket(socket),
      server_blocks()
  {
    server_blocks.push_back(server_block);
  }
} // namespace cache
