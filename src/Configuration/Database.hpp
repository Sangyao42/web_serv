#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "Configuration/Directive/Socket.hpp"
#include "Configuration/Directive/Block/Server.hpp"
#include "Configuration/Directive/Simple/MimeTypes.hpp"
#include "Configuration/Directive/Simple/AllowMethods.hpp"
#include "Configuration/Directive/Simple/Cgi.hpp"

class ConfigurationDatabase;
extern ConfigurationDatabase ws_database;

struct ServerQueryCache
{
  int                                   server_socket_fd;
  directive::Socket*                    socket;
  std::vector<directive::ServerBlock*>  server_blocks;
};

struct LocationQueryCache
{
  std::string                   match_path;
  directive::Methods            allowed_methods;
  std::string                   root;
  std::vector<std::string>      indexes;
  bool                          autoindex;
  std::vector<directive::Cgi*>  cgis;
  directive::MimeTypes          mime_types;
  std::string                   error_page;
  size_t                        client_max_body_size;
  std::string                   access_log;
  std::string                   error_log;
};

struct ConfigurationQueryResult
{
  std::vector<int>          path_to_location;
  directive::ServerBlock*   server_block;
  directive::LocationBlock* location_block;
  LocationQueryCache*       location_property;
};

class ConfigurationDatabase
{
  public:
    ConfigurationDatabase();
    ConfigurationDatabase(int cache_size);
    ConfigurationDatabase(const ConfigurationDatabase &other);
    ConfigurationDatabase &operator=(const ConfigurationDatabase &other);
    ~ConfigurationDatabase();

    size_t                                worker_connections() const;
    std::vector<directive::Socket>        all_server_sockets();

    void                                  register_server_socket(int server_socket_fd, const directive::Socket& socket);
    const ConfigurationQueryResult        query(int server_socket_fd, const std::string& server_name, const std::string& path);
    directive::ServerBlock*               server_blocks_of(int server_socket_fd, const std::string& server_name) const;

    void                                  set_location_cache_size(int size);

  private:
    std::vector<ServerQueryCache>         server_cache_;
    std::vector<LocationQueryCache>       location_cache_;
    int                                   location_insertion_index_;
};
