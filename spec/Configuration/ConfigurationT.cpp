#include "./ConfigurationT.hpp"

#include <cstdlib>

#include "Configuration.hpp"
#include "Configuration/Directive/Socket.hpp"
#include "Configuration/Directive/Block/Main.hpp"
#include "Configuration/Directive/Block/Http.hpp"
#include "Configuration/Directive/Block/Server.hpp"
#include "Configuration/Directive/Block/Location.hpp"
#include "Configuration/Directive/Block/Events.hpp"
#include "Configuration/Directive/Simple.hpp"
#include "Configuration/Directive/Simple/Listen.hpp"
#include "Configuration/Directive/Simple/ServerName.hpp"
#include "Configuration/Directive/Simple/Cgi.hpp"
#include "Configuration/Directive/Simple/Return.hpp"

TEST_F(TestConfiguration1, query_wrong_socket)
{
  const ConfigurationQueryResult result = config_.query(2, "hi.com", "/omg/what.html");
  ASSERT_TRUE(result.is_empty());
}

TEST_F(TestConfiguration1, query_server_block)
{
  ASSERT_EQ(config_.query_server_block(3, "hi.com"), &*main_block_->http().servers().first->second);
}

TEST_F(TestConfiguration1, query)
{
  const ConfigurationQueryResult result = config_.query(3, "hi.com", "/omg/what.html");
  ASSERT_FALSE(result.is_empty());

  ASSERT_EQ(result.location_block->type(), Directive::kDirectiveLocation);
  ASSERT_EQ(result.location_block->match(), "/omg/");
}

TEST_F(TestConfiguration1, query_use_first_server_if_no_server_name)
{
  const ConfigurationQueryResult result = config_.query(3, "", "/omg/what.html");
  ASSERT_FALSE(result.is_empty());

  ASSERT_EQ(result.location_block->type(), Directive::kDirectiveLocation);
  ASSERT_EQ(result.location_block->match(), "/omg/");
}

TEST_F(TestConfiguration1, query_use_first_server_if_wrong_server_name)
{
  const ConfigurationQueryResult result = config_.query(3, "non-existing.com", "/omg/what.html");
  ASSERT_FALSE(result.is_empty());

  ASSERT_EQ(result.location_block->type(), Directive::kDirectiveLocation);
  ASSERT_EQ(result.location_block->match(), "/omg/");
}

TEST_F(TestConfiguration1, query_correct_location_property)
{
  const ConfigurationQueryResult result = config_.query(3, "non-existing.com", "/develop/what.html");
  ASSERT_FALSE(result.is_empty());

  ASSERT_EQ(result.location_block->type(), Directive::kDirectiveLocation);
  ASSERT_EQ(result.location_block->match(), "/develop");
  ASSERT_EQ(result.location_property->server_block, &*main_block_->http().servers().first->second);
  ASSERT_EQ(result.location_property->match_path, "/develop");
  ASSERT_EQ(result.location_property->allowed_methods, directive::kMethodPost | directive::kMethodGet);
  ASSERT_EQ(result.location_property->client_max_body_size, static_cast<size_t>(1048576)); // 1M
  {
    const directive::Return*  return_ = result.location_property->redirect;
    ASSERT_TRUE(return_ != NULL);
    ASSERT_EQ(return_->get(), 301);
    ASSERT_EQ(return_->get_path(), "http://www.develop.com");
  }
  ASSERT_EQ(result.location_property->cgis.size(), static_cast<size_t>(0));
  ASSERT_EQ(result.location_property->indexes.size(), static_cast<size_t>(0));
  ASSERT_EQ(result.location_property->root, "");
  ASSERT_EQ(result.location_property->autoindex, true);
  ASSERT_EQ(result.location_property->mime_types, static_cast<void *>(0));
  ASSERT_EQ(result.location_property->error_pages.size(), static_cast<size_t>(0));
  ASSERT_EQ(result.location_property->access_log, "logs/access.log");
  ASSERT_EQ(result.location_property->error_log, "logs/error.log");
}

void  DoNothing(directive::MainBlock&) {}

/**
 * events {
 *  worker_connections 512;
 * }
 * 
 * http {
 *   error_log logs/error.log;
 *   access_log logs/access.log;
 *   server {
 *    listen 80;
 *    server_name hi.com wtf.fr;
 *    location / {
 *      root /var/www/html;
 *      index index.html index.htm default.html;
 * 
 *      location /omg/ {
 *        root /var/www/omg;
 *        cgi js /opt/homebrew/bin/node;
 *      }
 *    }
 *  
 *    autoindex on;
 *    location /images/ {
 *      root /var/www/images;
 *      cgi php /usr/local/bin/php4.3;
 *    }
 * 
 *    location /develop {
 *      return 301 http://www.develop.com;
 *    }
 * }
*/
void  Config1(directive::MainBlock& config)
{
  {
    directive::EventsBlock*  events = new directive::EventsBlock();
    config.add_directive(events);
    directive::WorkerConnections*  worker_connections = new directive::WorkerConnections(1024);
    worker_connections->set(512);
    events->add_directive(worker_connections);
  }
  directive::HttpBlock*    http = new directive::HttpBlock();
  config.add_directive(http);

  {
    directive::ErrorLog*  error_log = new directive::ErrorLog();
    error_log->set("logs/error.log");
    http->add_directive(error_log);
  }

  {
    directive::AccessLog*  access_log = new directive::AccessLog();
    access_log->set("logs/access.log");
    http->add_directive(access_log);
  }

  {
    directive::ServerBlock*  server = new directive::ServerBlock();
    http->add_directive(server);

    {
      directive::Listen*  listen = new directive::Listen();
      listen->add(directive::Socket("80"));
      server->add_directive(listen);
    }

    {
      directive::ServerName*  server_name = new directive::ServerName();
      server_name->add("hi.com");
      server_name->add("wtf.fr");
      server->add_directive(server_name);
    }

    {
      directive::LocationBlock*  location = new directive::LocationBlock();
      location->set("/");
      server->add_directive(location);

      {
        directive::Root*  root = new directive::Root();
        root->set("/var/www/html");
        location->add_directive(root);
      }

      {
        directive::Index*  index = new directive::Index();
        index->set("index.html");
        location->add_directive(index);
        index = new directive::Index();
        index->set("index.htm");
        location->add_directive(index);
        index = new directive::Index();
        index->set("default.html");
        location->add_directive(index);
      }
        
      {
        directive::LocationBlock*  location = new directive::LocationBlock();
        location->set("/omg/");
        server->add_directive(location);

        {
          directive::Root*  root = new directive::Root();
          root->set("/var/www/omg");
          location->add_directive(root);
        }

        {
          directive::Cgi*  cgi = new directive::Cgi();
          cgi->set("js", "/opt/homebrew/bin/node");
          location->add_directive(cgi);
        }
      }
    }

    {
      directive::Autoindex*  autoindex = new directive::Autoindex();
      autoindex->set(true);
      server->add_directive(autoindex);
    }

    {
      directive::LocationBlock*  location = new directive::LocationBlock();
      location->set("/images/");
      server->add_directive(location);

      {
        directive::Root*  root = new directive::Root();
        root->set("/var/www/images");
        location->add_directive(root);
      }

      {
        directive::Cgi*  cgi = new directive::Cgi();
        cgi->set("php", "/usr/local/bin/php4.3");
        location->add_directive(cgi);
      }
    }

    {
      directive::LocationBlock*  location = new directive::LocationBlock();
      location->set("/develop");
      server->add_directive(location);

      {
        directive::Return*  return_ = new directive::Return();
        return_->set("http://www.develop.com", 301);
        location->add_directive(return_);
      }
    }
  }
}

void  Config2(directive::MainBlock&)
{

}

void  Config3(directive::MainBlock&)
{

}
