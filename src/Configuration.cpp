#include "Configuration.hpp"

#include <cassert>

#include "misc/Maybe.hpp"
#include "Configuration.hpp"
#include "Configuration/Cache/LocationQuery.hpp"
#include "Configuration/Cache/ServerQuery.hpp"
#include "Configuration/Directive/Block/Server.hpp"
#include "Configuration/Directive/Block/Location.hpp"
#include "Configuration/Directive/Simple/Listen.hpp"
#include "Configuration/Directive/Simple/ServerName.hpp"

//////////////////////////////////////////////////////
////////////   ConfigurationQueryResult   ////////////
//////////////////////////////////////////////////////

ConfigurationQueryResult::ConfigurationQueryResult()
  : location_block(NULL),
    location_property(NULL) {}

ConfigurationQueryResult::ConfigurationQueryResult(const directive::LocationBlock* location_block,
                                                   cache::LocationQuery* location_property)
  : location_block(location_block),
    location_property(location_property) {}

bool  ConfigurationQueryResult::is_empty() const
{
  return location_block == NULL && location_property == NULL;
}

////////////////////////////////////////////
////////////   Configuration   /////////////
////////////////////////////////////////////

Configuration ws_configuration(16);

Configuration::Configuration()
  : server_cache_(),
    location_cache_(),
    location_insertion_index_(0),
    main_block_(NULL) {}

Configuration::Configuration(int cache_size)
  : server_cache_(),
    location_cache_(),
    location_insertion_index_(0),
    main_block_(NULL)
{
  location_cache_.reserve(cache_size);
}

Configuration::~Configuration()
{
  if (main_block_ != NULL)
    delete main_block_;
}

/////////////////////////////////////
////////////   setters   ////////////
/////////////////////////////////////

void  Configuration::register_server_socket(int server_socket_fd, const directive::Socket& socket)
{
  assert(!server_cache_.empty());
  for (std::vector<cache::ServerQuery>::iterator it = server_cache_.begin(); it != server_cache_.end(); ++it)
  {
    if (*it->socket == socket)
    {
      it->server_socket_fd = server_socket_fd;
      return;
    }
  }
}

void  Configuration::set_location_cache_size(int size)
{
  location_cache_.reserve(size);
}

void  Configuration::set_main_block(directive::MainBlock* main_block)
{
  main_block_ = main_block;
}

/////////////////////////////////////
////////////   getters   ////////////
/////////////////////////////////////

size_t Configuration::worker_connections() const
{
  assert(main_block_ != NULL);
  assert(main_block_->events().worker_connections().is_ok());
  return main_block_->events().worker_connections().value();
}

std::vector<const directive::Socket*> Configuration::all_server_sockets()
{
  if (server_cache_.empty())
    generate_server_cache();
  std::vector<const directive::Socket*> sockets;
  for (std::vector<cache::ServerQuery>::const_iterator it = server_cache_.begin(); it != server_cache_.end(); ++it)
  {
    if (it->socket != NULL)
    {
      sockets.push_back(it->socket);
    }
  }
  return sockets;
}

///////////////////////////////////////////
////////////   query methods   ////////////
///////////////////////////////////////////

const ConfigurationQueryResult  Configuration::query(int server_socket_fd,
                                                     const std::string& server_name,
                                                     const std::string& path)
{
  assert(main_block_ != NULL);
  const directive::ServerBlock* server_block = query_server_block(server_socket_fd, server_name);
  if (server_block == NULL)
    return ConfigurationQueryResult();
  const directive::LocationBlock* location_block = query_location_block(server_block, path);
  if (location_block == NULL)
    return ConfigurationQueryResult();
  // find the location property in the location cache
  for (std::vector<cache::LocationQuery>::iterator it = location_cache_.begin(); it != location_cache_.end(); ++it)
  {
    if (it->match_path == location_block->match() && it->server_block == server_block)
    {
      return ConfigurationQueryResult(location_block, &(*it));
    }
  }
  cache::LocationQuery* location_property = NULL;
  // if the location property is not found, then create a new location property
  if (location_cache_.size() == location_cache_.capacity())
    location_property = &location_cache_[location_insertion_index_];
  else
  {
    location_cache_.push_back(cache::LocationQuery());
    location_property = &location_cache_.back();
  }
  location_insertion_index_ = (location_insertion_index_ + 1) % location_cache_.size();
  // construct the configuration query result
  location_property->construct(server_block, location_block);
  return ConfigurationQueryResult(location_block, location_property);
}

const directive::LocationBlock*  Configuration::query_location_block(const directive::ServerBlock* server_block,
                                                                     const std::string& path) const
{
  assert(main_block_ != NULL);
  const directive::LocationBlock* result = NULL;
  // iterate over all location blocks
  directive::DirectivesRange  location_directives = server_block->query_directive(Directive::kDirectiveLocation);
  for (directive::DirectivesRange::first_type location_it = location_directives.first; location_it != location_directives.second; ++location_it)
  {
    assert(location_it->first == Directive::kDirectiveLocation);
    assert(location_it->second != NULL);
    const directive::LocationBlock* location_block = static_cast<const directive::LocationBlock*>(location_it->second);
    const directive::LocationBlock* match_result = location_block->best_match(path);
    if (match_result == NULL)
      continue;
    else if (result == NULL)
    {
      result = match_result;
    }
    else if (*result < *match_result)
    {
      result = match_result;
    }
  }
  return result;
}

const directive::ServerBlock*  Configuration::query_server_block(int server_socket_fd,
                                                                 const std::string& server_name) const
{
  assert(main_block_ != NULL);
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

Configuration::ServerBlocksQueryResult Configuration::query_server_blocks(int server_socket_fd) const
{
  assert(main_block_ != NULL);
  for (std::vector<cache::ServerQuery>::const_iterator it = server_cache_.begin(); it != server_cache_.end(); ++it)
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

void  Configuration::generate_server_cache()
{
  assert(main_block_ != NULL);
  assert(directive::DirectiveRangeIsValid(main_block_->http().servers()));
  directive::Servers servers = main_block_->http().servers();
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
        bool  found = false;
        for (std::vector<cache::ServerQuery>::iterator cache_it = server_cache_.begin();
             cache_it != server_cache_.end();
             ++cache_it)
        {
          if (*cache_it->socket == *socket_it)
          {
            cache_it->server_blocks.push_back(server_block);
            found = true;
            break;
          }
        }
        // otherwise, create a new server cache and add the server block to the server cache
        if (!found)
          server_cache_.push_back(cache::ServerQuery(&(*socket_it), server_block));
      }
    }
  }
}
