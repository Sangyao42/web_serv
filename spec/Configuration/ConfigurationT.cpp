#include "./ConfigurationT.hpp"

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
#include "Configuration/Directive/Simple/Redirect.hpp"

TEST_F(TestConfiguration1, query_non_exisiting_socket)
{
  Configuration config(8);

  config.set_main_block(main_block_);
  std::vector<const directive::Socket *> sockets = config.all_server_sockets();
  ASSERT_EQ(sockets.size(), static_cast<size_t>(1));
  ASSERT_EQ(*sockets[0], directive::Socket("80"));
  config.register_server_socket(3, *sockets[0]);
  const ConfigurationQueryResult result = config.query(2, "hi.com", "/omg/what.html");
  ASSERT_TRUE(result.is_empty());
}

TEST_F(TestConfiguration1, query)
{
  Configuration config(8);

  config.set_main_block(main_block_);
  std::vector<const directive::Socket *> sockets = config.all_server_sockets();
  ASSERT_EQ(sockets.size(), static_cast<size_t>(1));
  ASSERT_EQ(*sockets[0], directive::Socket("80"));
  config.register_server_socket(3, *sockets[0]);
  ASSERT_EQ(config.query_server_block(3, "hi.com"), &*main_block_->http().servers().first->second);
  const ConfigurationQueryResult result = config.query(3, "hi.com", "/omg/what.html");
  ASSERT_FALSE(result.is_empty());

  ASSERT_EQ(result.location_block->type(), Directive::kDirectiveLocation);
  ASSERT_EQ(result.location_block->match(), "/omg/");
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
        directive::Redirect*  return_ = new directive::Redirect();
        return_->set("http://www.develop.com", true);
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
