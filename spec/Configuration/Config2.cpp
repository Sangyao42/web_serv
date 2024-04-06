#include <cstdlib>

#include "constants.hpp"
#include "Configuration.hpp"
#include "Uri/Authority.hpp"
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
 *   }
 * 
 *   server {
 *     listen 127.0.0.1;
 *     server_name whatt.42.fr;
 *     root /Users/Anthony;
 *   }
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
      listen->add(uri::Authority("198.13.20.5", "443"));
      server->add_directive(listen);
    }
    {
      directive::Listen*  listen = new directive::Listen();
      listen->add(uri::Authority("::", "443"));
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
      listen->add(uri::Authority("127.0.0.1"));
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

  // server 4
  {
    directive::ServerBlock*  server = new directive::ServerBlock();
    http->add_directive(server);

    {
      directive::Listen*  listen = new directive::Listen();
      listen->add(uri::Authority("127.0.0.1"));
      server->add_directive(listen);
    }

    {
      directive::ServerName*  server_name = new directive::ServerName();
      server_name->add("whatt.42.fr");
      server->add_directive(server_name);
    }

    {
      directive::Root*  root = new directive::Root();
      root->set("/Users/Anthony");
      server->add_directive(root);
    }
  }
}
