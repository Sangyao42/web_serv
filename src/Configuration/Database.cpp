#include "Database.hpp"

#include <cassert>

#include "misc/Maybe.hpp"
#include "Configuration.hpp"
#include "Configuration/Directive/Block/Server.hpp"
#include "Configuration/Directive/Block/Location.hpp"
#include "Configuration/Directive/Simple/Listen.hpp"
#include "Configuration/Directive/Simple/ServerName.hpp"

//////////////////////////////////////////////////////
////////////   ConfigurationQueryResult   ////////////
//////////////////////////////////////////////////////

ConfigurationQueryResult::ConfigurationQueryResult()
  : server_block(NULL),
    location_block(NULL),
    location_property(NULL) {}

ConfigurationQueryResult::ConfigurationQueryResult(const directive::ServerBlock* server_block,
                                                   const directive::LocationBlock* location_block,
                                                   LocationQueryCache* location_property)
  : server_block(server_block),
    location_block(location_block),
    location_property(location_property) {}

bool  ConfigurationQueryResult::is_empty() const
{
  return server_block == NULL && location_block == NULL && location_property == NULL;
}

/////////////////////////////////////////////
////////////   SeverQueryCache   ////////////
/////////////////////////////////////////////

ServerQueryCache::ServerQueryCache()
  : server_socket_fd(-1),
    socket(NULL),
    server_blocks() {}

ServerQueryCache::ServerQueryCache(const directive::Socket* socket,
                                   const directive::ServerBlock* server_block)
  : server_socket_fd(-1),
    socket(socket),
    server_blocks()
{
  server_blocks.push_back(server_block);
}

////////////////////////////////////////////////////
////////////   ConfigurationDatabase   /////////////
////////////////////////////////////////////////////

ConfigurationDatabase ws_database;

ConfigurationDatabase::ConfigurationDatabase()
  : server_cache_(),
    location_cache_(),
    location_insertion_index_(0) {}

ConfigurationDatabase::ConfigurationDatabase(int cache_size)
  : server_cache_(),
    location_cache_(),
    location_insertion_index_(0)
{
  location_cache_.reserve(cache_size);
}

ConfigurationDatabase::ConfigurationDatabase(const ConfigurationDatabase &other)
  : server_cache_(other.server_cache_),
    location_cache_(other.location_cache_),
    location_insertion_index_(other.location_insertion_index_) {}

ConfigurationDatabase &ConfigurationDatabase::operator=(const ConfigurationDatabase &other)
{
  server_cache_ = other.server_cache_;
  location_cache_ = other.location_cache_;
  location_insertion_index_ = other.location_insertion_index_;
  return *this;
}

ConfigurationDatabase::~ConfigurationDatabase() {}

size_t ConfigurationDatabase::worker_connections() const
{
  assert(ws_configuration.events.worker_connections().is_ok());
  return ws_configuration.events.worker_connections().value();
}

std::vector<const directive::Socket*> ConfigurationDatabase::all_server_sockets()
{
  if (server_cache_.empty())
    generate_server_cache();
  std::vector<const directive::Socket*> sockets;
  for (std::vector<ServerQueryCache>::const_iterator it = server_cache_.begin(); it != server_cache_.end(); ++it)
  {
    if (it->socket != NULL)
    {
      sockets.push_back(it->socket);
    }
  }
  return sockets;
}

void  ConfigurationDatabase::register_server_socket(int server_socket_fd, const directive::Socket& socket)
{
  for (std::vector<ServerQueryCache>::iterator it = server_cache_.begin(); it != server_cache_.end(); ++it)
  {
    if (*it->socket == socket)
    {
      it->server_socket_fd = server_socket_fd;
      return;
    }
  }
}

void  ConfigurationDatabase::set_location_cache_size(int size)
{
  location_cache_.reserve(size);
}

///////////////////////////////////////////
////////////   query methods   ////////////
///////////////////////////////////////////

const ConfigurationQueryResult  ConfigurationDatabase::query(int server_socket_fd,
                                                             const std::string& server_name,
                                                             const std::string& path)
{
  const directive::ServerBlock* server_block = query_server_block(server_socket_fd, server_name);
  if (server_block == NULL)
    return ConfigurationQueryResult();
  const directive::LocationBlockMatchResult location_block_match = query_location_block(server_block, path);
  if (location_block_match == directive::LocationBlockMatchResult())
    return ConfigurationQueryResult();
  const directive::LocationBlockPath& location_path = location_block_match.first;
  const directive::LocationBlock* location_block = location_block_match.second;
  LocationQueryCache* location_property = NULL;
  // find the location property in the location cache
  for (std::vector<LocationQueryCache>::iterator it = location_cache_.begin(); it != location_cache_.end(); ++it)
  {
    if (it->match_path == location_block->match())
    {
      location_property = &(*it);
      break;
    }
  }
  // if the location property is not found, then create a new location property
  if (location_property == NULL)
  {
    location_cache_.push_back(LocationQueryCache());
    location_property = &location_cache_.back();
  }
  // construct the configuration query result
  location_property->construct(location_path, location_block, server_block);
  return ConfigurationQueryResult(server_block, location_block, location_property);
}

const directive::LocationBlockMatchResult  ConfigurationDatabase::query_location_block(const directive::ServerBlock* server_block,
                                                                                       const std::string& path) const
{
  directive::LocationBlockMatchResult result;
  // iterate over all location blocks
  directive::DirectivesRange  location_directives = server_block->query_directive(Directive::kDirectiveLocation);
  for (directive::DirectivesRange::first_type location_it = location_directives.first; location_it != location_directives.second; ++location_it)
  {
    assert(location_it->first == Directive::kDirectiveLocation);
    assert(location_it->second != NULL);
    const directive::LocationBlock* location_block = static_cast<const directive::LocationBlock*>(location_it->second);
    directive::LocationBlockMatchResult match_result = location_block->best_match(path);
    if (result < match_result)
    {
      result = match_result;
    }
  }
  if (result != directive::LocationBlockMatchResult())
  {
    result.first.insert(result.first.begin(), server_block->index());
  }
  return result;
}

const directive::ServerBlock*  ConfigurationDatabase::query_server_block(int server_socket_fd,
                                                                         const std::string& server_name) const
{
  ServerBlocksQueryResult server_blocks = query_server_blocks(server_socket_fd);
  if (!server_blocks.is_ok())
    return NULL;
  // iterate over all server blocks
  for (std::vector<const directive::ServerBlock*>::const_iterator it = server_blocks.value()->begin(); it != server_blocks.value()->end(); ++it)
  {
    directive::DirectivesRange  server_name_directive = (*it)->query_directive(Directive::kDirectiveServerName);

    // iterate over all server name directives
    for (directive::DirectivesRange::first_type server_name_it = server_name_directive.first; server_name_it != server_name_directive.second; ++server_name_it)
    {
      assert(server_name_it->first == Directive::kDirectiveServerName);
      assert(server_name_it->second != NULL);
      const directive::ServerName* server_name_directive = static_cast<const directive::ServerName*>(server_name_it->second);
      const std::vector<std::string>& server_names = server_name_directive->get();

      // iterate over all server names in a server name directive
      for (std::vector<std::string>::const_iterator server_name_it = server_names.begin(); server_name_it != server_names.end(); ++server_name_it)
      {
        if (*server_name_it == server_name)
        {
          return *it;
        }
      }
    }
  }
  return NULL;
}

ConfigurationDatabase::ServerBlocksQueryResult ConfigurationDatabase::query_server_blocks(int server_socket_fd) const
{
  for (std::vector<ServerQueryCache>::const_iterator it = server_cache_.begin(); it != server_cache_.end(); ++it)
  {
    if (it->server_socket_fd == server_socket_fd)
    {
      return &it->server_blocks;
    }
  }
  return Nothing();
}

/////////////////////////////////////////////
////////////   private methods   ////////////
/////////////////////////////////////////////

void  ConfigurationDatabase::generate_server_cache()
{
  assert(ws_configuration.http.servers().is_ok());
  directive::Servers servers = ws_configuration.http.servers().value();
  // iterate over all server blocks
  for (directive::Servers::first_type it = servers.first; it != servers.second; ++it)
  {
    assert(it->first == Directive::kDirectiveServer);
    assert(it->second != NULL);
    const directive::ServerBlock* server_block = static_cast<const directive::ServerBlock*>(it->second);
    const directive::DirectivesRange listen_directives = server_block->query_directive(Directive::kDirectiveListen);
    assert(directive::DirectiveRangeIsValid(listen_directives));

    // iterate over all listen directives
    for (directive::DirectivesRange::first_type listen_it = listen_directives.first; listen_it != listen_directives.second; ++listen_it)
    {
      assert(listen_it->first == Directive::kDirectiveListen);
      assert(listen_it->second != NULL);
      const directive::Listen* listen = static_cast<const directive::Listen*>(listen_it->second);
      const std::vector<directive::Socket>& sockets = listen->get();
      assert(!sockets.empty());

      // iterate over all sockets in a listen directive
      for (std::vector<directive::Socket>::const_iterator socket_it = sockets.begin(); socket_it != sockets.end(); ++socket_it)
      {
        // if the server cache that has the same socket, then add the server block to the server cache
        std::vector<ServerQueryCache>::iterator cache_it = server_cache_.begin();
        for (; cache_it != server_cache_.end(); ++cache_it)
        {
          if (*cache_it->socket == *socket_it)
          {
            cache_it->server_blocks.push_back(server_block);
            break;
          }
        }
        // otherwise, create a new server cache and add the server block to the server cache
        if (cache_it == server_cache_.end())
        {
          server_cache_.push_back(ServerQueryCache(&(*socket_it), server_block));
        }
      }
    }
  }
}
