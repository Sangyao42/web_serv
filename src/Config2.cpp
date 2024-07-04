#include <cstdlib>

#include "Configuration/Directive/Simple/AllowMethods.hpp"
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
      listen->add(uri::Authority("localhost", "8080"));
      server->add_directive(listen);
    }
    {
      directive::Listen*  listen = new directive::Listen();
      listen->add(uri::Authority("::", "8088"));
      server->add_directive(listen);
    }
    {
      directive::ServerName*  server_name = new directive::ServerName();
      server_name->add("localhost");
      server->add_directive(server_name);
    }
    {
      directive::Root*  root = new directive::Root();
      root->set("./www");
      server->add_directive(root);
    }

    {
      directive::LocationBlock*  location = new directive::LocationBlock();
      location->set("/display");
      server->add_directive(location);
	  {
      directive::Cgi* python = new directive::Cgi();
      python->set("py", "/usr/bin/python3");
      location->add_directive(python);
      }

      {
        directive::Root*  root = new directive::Root();
        root->set("./www");
        location->add_directive(root);
      }

	  {
      directive::Autoindex*  autoindex = new directive::Autoindex();
      autoindex->set(true);
      location->add_directive(autoindex);
      }
	}

	{
      directive::LocationBlock*  location = new directive::LocationBlock();
      location->set("/upload");
      server->add_directive(location);

      {
        directive::Root*  root = new directive::Root();
        root->set("./www");
        location->add_directive(root);
      }

	  {
      directive::Autoindex*  autoindex = new directive::Autoindex();
      autoindex->set(true);
      location->add_directive(autoindex);
      }

	  {
	  directive::AllowMethods*  allow_methods = new directive::AllowMethods();
	  allow_methods->set(directive::kMethodPost | directive::kMethodGet | directive::kMethodDelete);
	  location->add_directive(allow_methods);
	  }

	  {
		directive::MimeTypes* mime_types = new directive::MimeTypes();
		mime_types->add("txt", "text/plain");
		mime_types->add("html", "text/html");
		location->add_directive(mime_types);
	  }

	}

	{
      directive::LocationBlock*  location = new directive::LocationBlock();
      location->set("/register");
      server->add_directive(location);
	  {
      directive::Cgi* python = new directive::Cgi();
      python->set("py", "/usr/bin/python3");
      location->add_directive(python);
      }

      {
        directive::Root*  root = new directive::Root();
        root->set("./www");
        location->add_directive(root);
      }

	  {
	  directive::AllowMethods*  allow_methods = new directive::AllowMethods();
	  allow_methods->set(directive::kMethodGet | directive::kMethodPost);
	  location->add_directive(allow_methods);
	  }

	  {
		directive::MimeTypes* mime_types = new directive::MimeTypes();
		mime_types->add("txt", "text/plain");
		mime_types->add("html", "text/html");
		mime_types->add("json", "application/json");
		location->add_directive(mime_types);
	  }

	  {
      directive::Autoindex*  autoindex = new directive::Autoindex();
      autoindex->set(true);
      location->add_directive(autoindex);
      }
	}

    {
      directive::Autoindex*  autoindex = new directive::Autoindex();
      autoindex->set(true);
      server->add_directive(autoindex);
    }
  }

//   // server 2
//   {
//     directive::ServerBlock*  server = new directive::ServerBlock();
//     http->add_directive(server);

//     {
//       directive::Root*  root = new directive::Root();
//       root->set("what/hello/");
//       server->add_directive(root);
//     }

//     {
//       directive::Cgi*  cgi = new directive::Cgi();
//       cgi->set("php", "/usr/local/bin/php4.3");
//       server->add_directive(cgi);
//     }

//     {
//       directive::Cgi*  cgi = new directive::Cgi();
//       cgi->set("js", "/opt/homebrew/bin/node");
//       server->add_directive(cgi);
//     }

//     {
//       directive::Cgi*  cgi = new directive::Cgi();
//       cgi->set("php", "/usr/local/bin/php4.8");
//       server->add_directive(cgi);
//     }
//   }

//   // server 3
//   {
//     directive::ServerBlock*  server = new directive::ServerBlock();
//     http->add_directive(server);

//     {
//       directive::Listen*  listen = new directive::Listen();
//       listen->add(uri::Authority("127.0.0.1"));
//       server->add_directive(listen);
//     }

//     {
//       directive::ServerName*  server_name = new directive::ServerName();
//       server_name->add("wtf.fr");
//       server->add_directive(server_name);
//     }

    // {
    //   directive::Autoindex*  autoindex = new directive::Autoindex();
    //   autoindex->set(true);
    //   server->add_directive(autoindex);
    // }
//   }

//   // server 4
//   {
//     directive::ServerBlock*  server = new directive::ServerBlock();
//     http->add_directive(server);

//     {
//       directive::Listen*  listen = new directive::Listen();
//       listen->add(uri::Authority("127.0.0.1"));
//       server->add_directive(listen);
//     }

//     {
//       directive::ServerName*  server_name = new directive::ServerName();
//       server_name->add("whatt.42.fr");
//       server->add_directive(server_name);
//     }

//     {
//       directive::Root*  root = new directive::Root();
//       root->set("/Users/Anthony");
//       server->add_directive(root);
//     }
//   }
}
