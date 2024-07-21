# Web Server

> A HTTP server written in C++ from scratch

## Design

> In construction (see [the current plan](docs/planning.md))

## Specification

### Supported HTTP Fields

The syntax for each fields are specified in [necessary_fields.abnf](docs/HTTP_fields/necessary_fields.abnf)

Fields when the HTTP message has a body:

- Content-Type
- Content-Length
- Content-Location

Fields in both HTTP request and response:

- Connection

Fields only in HTTP request:

- Host
- Referer

Fields only in HTTP response:

- Last-Modified
- Allow
- Location
- Server
- User-Agent

Grammer for the HTTP parser can be read [here](docs/HTTP.abnf)

### Supported server configuration directives

```nginx
http {
     types {
        text/html html;
        text/css css;
        text/javascript js;
        image/png png;
        image/jpeg jpeg;
        image/gif gif;
        application/json json;
    }

    server {
        listen 8080;
        server_name my.com;
        root /var/www/html;
        index index.html;
        autoindex on;
        error_pages 404 /404.html;
        access_log /var/log/access.log;
        error_log /var/log/error.log;
        location / {
            cgi py /cgi-bin;
        }
        location /images {
            allow_methods POST DELETE;
            return 301 /new;
        }
        location /images2 {
            return 307 https://www.google.com;
        }
        location /cgi-bin {
            allow_methods DELETE;
            return 200 "you are welcome";
        }
		location /hello {
			return 404;
		}
    }
}
```

#### Block directives

- `http` - The http block of the server
- `server` - The server block of the server
- `events` - The events block of the server
- `location` - The location block of the server

#### Directives related to HTTP requests

- `listen` - The port that the server will listen on
- `server_name` - The server name of the server
- `allow_methods` - Specify the allowed methods

#### Directives for HTTP response generation

Generating a response from a requested file:

- `root` - The root directory of the server.
- `index` - The index files of the server. If autonindex is off, a HTTP request that ends with a '/' will try to return the first found index file instead.
- `types` - Map response file name extensions to MIME types
- `error_pages` - The error page of the server

Generating a response by other means:

- `client_max_body_size` - The maximum body size of the client. If the size in a request exceeds the configured value, the 413 (Request Entity Too Large) error is returned to the client.
- `return` - returns a response with the specified status code. Redirections are handled by this directive: temporily (307) or permanantly (301) redirection. If the status code is not a redirection, it can also return a string that will be sent in the HTTP response body. Return has the highest priority than the `cgi` directive.
- `autoindex` - Specify whether to show directory listings
- `cgi` - Specify a cgi script

#### Misc directives

For logging information:

- `access_log` - The access log file of the server
- `error_log` - The error log file of the server

- `worker_connections` - Maximum amount of connections that the server will handle at any given point of time 

See the properties of all supported directives [here](docs/planning.md#configuration-file)
Grammer for the configuration parser can be read [here](docs/Config.abnf)

## External materials

- [Memory allocation strategies](https://www.gingerbill.org/series/memory-allocation-strategies/)
- [RFC 9112](https://datatracker.ietf.org/doc/html/rfc9112)

# Collaboration notes

Accepted git commit message tags:

- feat
- fix
- chore
- perf
- refactor
- doc

Code should follow the [google cpp style](https://anthonytsang.notion.site/Google-C-code-guidelines-ce6a361b17a5415bb41ab264f1866e75?pvs=4)