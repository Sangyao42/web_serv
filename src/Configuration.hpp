#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "Uri/Authority.hpp"
#include "Configuration/Cache/LocationQuery.hpp"
#include "Configuration/Cache/ServerQuery.hpp"
#include "Configuration/Directive/Block/Main.hpp"
#include "Configuration/Directive/Block/Server.hpp"
#include "Configuration/Directive/Block/Location.hpp"

class Configuration;
extern Configuration ws_database;

struct ConfigurationQueryResult
{
  const directive::LocationBlock* location_block;
  cache::LocationQuery*           location_property;

  ConfigurationQueryResult();
  ConfigurationQueryResult(const directive::LocationBlock* location_block,
                           cache::LocationQuery* location_property);

  bool  is_empty() const;
};

class Configuration
{
  public:
    typedef Maybe<const std::vector<const directive::ServerBlock*>*>  ServerBlocksQueryResult;

    Configuration();
    Configuration(int cache_size);
    ~Configuration();

    ///////////////////////////////////////////////
    ////////////   setup this object   ////////////
    ///////////////////////////////////////////////

    // Set the size of the location cache.
    void  set_location_cache_size(int size);

    // Set the main block of the configuration.
    void  set_main_block(directive::MainBlock* main_block);

    // Get all the server sockets.
    std::vector<const uri::Authority*>    all_server_sockets();

    // After getting all the server sockets, one has to register them with the socket file descriptor
    // associated with the parsed socket object.
    void                                  register_server_socket(int server_socket_fd,
                                                                 const uri::Authority& socket);

    /////////////////////////////////////
    ////////////   getters   ////////////
    /////////////////////////////////////

    size_t                                worker_connections() const;

    ///////////////////////////////////////////
    ////////////   query methods   ////////////
    ///////////////////////////////////////////

    const ConfigurationQueryResult        query(int server_socket_fd,
                                                const std::string& server_name,
                                                const std::string& path);

    const directive::LocationBlock*       query_location_block(const directive::ServerBlock* server_block,
                                                               const std::string& path) const;
    const directive::ServerBlock*         query_server_block(int server_socket_fd,
                                                             const std::string& server_name) const;
    ServerBlocksQueryResult               query_server_blocks(int server_socket_fd) const;

  private:
    std::vector<cache::ServerQuery>       server_cache_;
    std::vector<cache::LocationQuery>     location_cache_;
    int                                   location_insertion_index_;
    directive::MainBlock*                 main_block_;

    void                                  generate_server_cache();
    void                                  add_unique_server_cache(const uri::Authority* socket,
                                                                  const directive::ServerBlock* server_block);

    Configuration(const Configuration &other);
    Configuration &operator=(const Configuration &other);
};
