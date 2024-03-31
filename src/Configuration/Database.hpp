#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "misc/Maybe.hpp"
#include "Configuration/Directive/Socket.hpp"
#include "Configuration/Directive/Block/Server.hpp"
#include "Configuration/Directive/Block/Location.hpp"
#include "Configuration/Directive/Simple/MimeTypes.hpp"
#include "Configuration/Directive/Simple/AllowMethods.hpp"
#include "Configuration/Directive/Simple/Redirect.hpp"
#include "Configuration/Directive/Simple/Cgi.hpp"

class ConfigurationDatabase;
extern ConfigurationDatabase ws_database;

/**
 * @brief Properties of a location block
*/
struct LocationQueryCache
{
  std::string                   match_path;
  // direvtives to decide if the request is allowed
  directive::Methods            allowed_methods;
  size_t                        client_max_body_size;
  // If redirect is not null, the request should be generated a redirect response
  directive::Redirect*          redirect;
  // If cgi is not null, the request should be handled by a cgi script
  std::vector<directive::Cgi*>  cgis;
  // directives to find the full path of the requested resource
  std::string                   root;
  std::vector<std::string>      indexes;
  bool                          autoindex;
  directive::MimeTypes          mime_types;
  std::string                   error_page;
  std::string                   access_log;
  std::string                   error_log;

  LocationQueryCache();
};

struct ConfigurationQueryResult
{
  const directive::ServerBlock*   server_block;
  const directive::LocationBlock* location_block;
  LocationQueryCache*             location_property;

  ConfigurationQueryResult();
  ConfigurationQueryResult(const directive::ServerBlock* server_block,
                           const directive::LocationBlock* location_block,
                           LocationQueryCache* location_property);

  bool  is_empty() const;
};

struct ServerQueryCache
{
  int                                         server_socket_fd;
  const directive::Socket*                    socket;
  std::vector<const directive::ServerBlock*>  server_blocks;

  ServerQueryCache();
  ServerQueryCache(const directive::Socket* socket, const directive::ServerBlock* server_block);
};

class ConfigurationDatabase
{
  public:
    typedef Maybe<const std::vector<const directive::ServerBlock*>*>  ServerBlocksQueryResult;

    ConfigurationDatabase();
    ConfigurationDatabase(int cache_size);
    ConfigurationDatabase(const ConfigurationDatabase &other);
    ConfigurationDatabase &operator=(const ConfigurationDatabase &other);
    ~ConfigurationDatabase();

    size_t                                worker_connections() const;
    std::vector<const directive::Socket*> all_server_sockets();
    void                                  register_server_socket(int server_socket_fd,
                                                                 const directive::Socket& socket);

    void                                  set_location_cache_size(int size);

    const ConfigurationQueryResult        query(int server_socket_fd,
                                                const std::string& server_name,
                                                const std::string& path);

    const directive::LocationBlockMatchResult query_location_block(const directive::ServerBlock* server_block,
                                                               const std::string& path) const;
    const directive::ServerBlock*         query_server_block(int server_socket_fd,
                                                             const std::string& server_name) const;
    ServerBlocksQueryResult               query_server_blocks(int server_socket_fd) const;

    void                                  construct_location_property(LocationQueryCache* location_property,
                                                                      const directive::LocationBlockPath& location_block_path,
                                                                      const directive::LocationBlock* location_block,
                                                                      const directive::ServerBlock* server_block);

  private:
    std::vector<ServerQueryCache>         server_cache_;
    std::vector<LocationQueryCache>       location_cache_;
    int                                   location_insertion_index_;

    void                                  generate_server_cache();
};
