#include "./ConfigurationT.hpp"

#include <cstdlib>

#include "constants.hpp"
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

TEST(TestConfiguration0, all_server_sockets)
{
  Configuration config(8);
  EXPECT_DEATH({
    std::vector<const directive::Socket *> sockets = config.all_server_sockets();
  }, "Assertion.*");
}

TEST(TestConfiguration0, query_no_main_block)
{
  Configuration config(8);

  ConfigurationQueryResult result = config.query(3, "hi.com", "/omg/what.html");
  ASSERT_TRUE(result.is_empty());
}

TEST(TestConfiguration0, query_empty)
{
  Configuration config(8);

  config.set_main_block(new directive::MainBlock());
  ConfigurationQueryResult result = config.query(3, "hi.com", "/omg/what.html");
  ASSERT_TRUE(result.is_empty());
}

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
  ASSERT_EQ(result.location_property->allowed_methods, constants::kDefaultAllowedMethods);
  ASSERT_EQ(result.location_property->client_max_body_size, constants::kDefaultClientMaxBodySize); // 1M
  {
    const directive::Return*  return_ = result.location_property->redirect;
    ASSERT_TRUE(return_ != NULL);
    ASSERT_EQ(return_->get(), 301);
    ASSERT_EQ(return_->get_path(), "http://www.develop.com");
  }
  ASSERT_EQ(result.location_property->cgis.size(), static_cast<size_t>(0));
  ASSERT_EQ(result.location_property->indexes.size(), static_cast<size_t>(1));
  ASSERT_EQ(result.location_property->indexes[0], &constants::kDefaultIndex);
  ASSERT_EQ(result.location_property->root, constants::kDefaultRoot);
  ASSERT_EQ(result.location_property->autoindex, true);
  ASSERT_EQ(result.location_property->mime_types, &constants::kDefaultMimeTypes);
  ASSERT_EQ(result.location_property->error_pages.size(), static_cast<size_t>(0));
  ASSERT_EQ(result.location_property->access_log, "/var/logs/access.log");
  ASSERT_EQ(result.location_property->error_log, "/var/logs/error.log");
}

TEST_F(TestConfiguration1, query_correct_location_property2)
{
  const ConfigurationQueryResult result = config_.query(3, "non-existing.com", "/omg/");
  ASSERT_FALSE(result.is_empty());

  ASSERT_EQ(result.location_block->type(), Directive::kDirectiveLocation);
  ASSERT_EQ(result.location_block->match(), "/omg/");
  ASSERT_EQ(result.location_property->server_block, &*main_block_->http().servers().first->second);
  ASSERT_EQ(result.location_property->match_path, "/omg/");
  ASSERT_EQ(result.location_property->allowed_methods, constants::kDefaultAllowedMethods);
  ASSERT_EQ(result.location_property->client_max_body_size, constants::kDefaultClientMaxBodySize); // 1M
  ASSERT_TRUE(result.location_property->redirect == NULL);
  {
    std::vector<const directive::Cgi*>*  cgis = &result.location_property->cgis;
    ASSERT_EQ(cgis->size(), static_cast<size_t>(1));
    ASSERT_EQ((*cgis)[0]->get(), "js");
    ASSERT_EQ((*cgis)[0]->extension(), "js");
    ASSERT_EQ((*cgis)[0]->cgi_path(), "/opt/homebrew/bin/node");
  }
  {
    std::vector<const directive::Index*>*  indexes = &result.location_property->indexes;
    ASSERT_EQ(indexes->size(), static_cast<size_t>(3));
    ASSERT_EQ((*indexes)[0]->get(), "index.html");
    ASSERT_EQ((*indexes)[1]->get(), "index.htm");
    ASSERT_EQ((*indexes)[2]->get(), "default.html");
  }
  ASSERT_EQ(result.location_property->root, "/var/www/omg");
  ASSERT_EQ(result.location_property->autoindex, constants::kDefaultAutoindex);
  ASSERT_EQ(result.location_property->mime_types, &constants::kDefaultMimeTypes);
  ASSERT_EQ(result.location_property->error_pages.size(), static_cast<size_t>(0));
  ASSERT_EQ(result.location_property->access_log, "/var/logs/access.log");
  ASSERT_EQ(result.location_property->error_log, "/var/logs/error.log");
}

/**
 * events {
 *  worker_connections 512;
 * }
 * 
 * http {
 *   error_log /var/logs/error.log;
 *   access_log /var/logs/access.log;
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
    error_log->set("/var/logs/error.log");
    http->add_directive(error_log);
  }

  {
    directive::AccessLog*  access_log = new directive::AccessLog();
    access_log->set("/var/logs/access.log");
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
        // Construction of multiple indexes on the same line
        // has to be done in reverse order.
        directive::Index*  index = new directive::Index();
        index->set("default.html");
        location->add_directive(index);
        index = new directive::Index();
        index->set("index.htm");
        location->add_directive(index);
        index = new directive::Index();
        index->set("index.html");
        location->add_directive(index);
      }
        
      {
        directive::LocationBlock*  location2 = new directive::LocationBlock();
        location2->set("/omg/");
        location->add_directive(location2);

        {
          directive::Root*  root = new directive::Root();
          root->set("/var/www/omg");
          location2->add_directive(root);
        }

        {
          directive::Cgi*  cgi = new directive::Cgi();
          cgi->set("js", "/opt/homebrew/bin/node");
          location2->add_directive(cgi);
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

TEST_F(TestConfiguration2, query_wrong_socket)
{
  const ConfigurationQueryResult result = config_.query(2, "hi.com", "/omg/what.html");
  ASSERT_TRUE(result.is_empty());
}

TEST_F(TestConfiguration2, query_server_block)
{
  ASSERT_EQ(config_.query_server_block(3, "hi.com"), &*main_block_->http().servers().first->second);
}

/**
 * http {
 *   server {
 *     listen 198.13.20.5:443;
 *     listen [;;]:443;
 *     server_name hi.com;
 *     root /var/www/original;
 * 
 *     location /images {
 *       root /var/example;
 *     }
 *   }
 * 
 *   server {
 *     root what/hello/;
 *     cgi php /usr/local/bin/php4.3;
 *     cgi js /opt/homebrew/bin/node;
 *     cgi php /usr/local/bin/php4.8;
 *   }
 * 
 *   server {
 *      listen 127.0.0.1;
 *      server_name wtf.fr;
 *      autoindex on;
 *   }}
 * }
*/
void  Config2(directive::MainBlock& main)
{
  directive::HttpBlock*    http = new directive::HttpBlock();
  main.add_directive(http);

  // server 1
  {
    directive::ServerBlock*  server = new directive::ServerBlock();
    http->add_directive(server);

    {
      directive::Listen*  listen = new directive::Listen();
      listen->add(directive::Socket("198.13.20.5", "443"));
      server->add_directive(listen);
    }
    {
      directive::Listen*  listen = new directive::Listen();
      listen->add(directive::Socket("::", "443"));
      server->add_directive(listen);
    }
    {
      directive::ServerName*  server_name = new directive::ServerName();
      server_name->add("hi.com");
      server->add_directive(server_name);
    }
    {
      directive::Root*  root = new directive::Root();
      root->set("/var/www/original");
      server->add_directive(root);
    }

    {
      directive::LocationBlock*  location = new directive::LocationBlock();
      location->set("/images");
      server->add_directive(location);

      {
        directive::Root*  root = new directive::Root();
        root->set("/var/example");
        location->add_directive(root);
      }
    }
  }

  // server 2
  {
    directive::ServerBlock*  server = new directive::ServerBlock();
    http->add_directive(server);

    {
      directive::Root*  root = new directive::Root();
      root->set("what/hello/");
      server->add_directive(root);
    }

    {
      directive::Cgi*  cgi = new directive::Cgi();
      cgi->set("php", "/usr/local/bin/php4.3");
      server->add_directive(cgi);
    }

    {
      directive::Cgi*  cgi = new directive::Cgi();
      cgi->set("js", "/opt/homebrew/bin/node");
      server->add_directive(cgi);
    }

    {
      directive::Cgi*  cgi = new directive::Cgi();
      cgi->set("php", "/usr/local/bin/php4.8");
      server->add_directive(cgi);
    }
  }

  // server 3
  {
    directive::ServerBlock*  server = new directive::ServerBlock();
    http->add_directive(server);

    {
      directive::Listen*  listen = new directive::Listen();
      listen->add(directive::Socket("127.0.0.1"));
      server->add_directive(listen);
    }

    {
      directive::ServerName*  server_name = new directive::ServerName();
      server_name->add("wtf.fr");
      server->add_directive(server_name);
    }

    {
      directive::Autoindex*  autoindex = new directive::Autoindex();
      autoindex->set(true);
      server->add_directive(autoindex);
    }
  }
}

void  Config3(directive::MainBlock&)
{

}
