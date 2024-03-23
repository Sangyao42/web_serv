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
- User-Agent
- Authorization (optional)
- Accept (optional)

Fields only in HTTP response:

- Last-Modified
- Allow
- Location
- Retry-After (maybe for CGI)
- Server
- WWW-Authenticate, Authentication-Info (optional)

### Supported server configuration directives

```nginx
types {
    text/html html;
    text/css css;
    text/javascript js;
    image/png png;
    image/jpeg jpeg;
    image/gif gif;
    application/json json;
}

http {
    include mime.types;
    server {
        listen 8080;
        server_name my.com;
        root /var/www/html;
        index index.html;
        autoindex on;
        error_page 404 /404.html;
        access_log /var/log/access.log;
        error_log /var/log/error.log;
        location / {
            allow GET POST;
            cgi py /cgi-bin;
        }
        location /images {
			deny_methods GET;
            redirect /new permanent;
        }
        location /cgi-bin {
            allow_methods DELETE;
            redirect /new redirect;
        }
    }
}
```

- `http` - The http block of the server
- `server` - The server block of the server
- `events` - The events block of the server
- `location` - The location block of the server

- `types` - Map response file name extensions to MIME types
- `listen` - The port the server will listen on
- `root` - The root directory of the server
- `index` - The index file of the server
- `autoindex` - Specify whether to show directory listings
- `error_page` - The error page of the server
- `access_log` - The access log file of the server
- `error_log` - The error log file of the server
- `server_name` - The server name of the server
- `client_max_body_size` - The maximum body size of the client
- `include` - Include another configuration file
- `cgi` - Specify a cgi script
- `allow_methods` - Specify the allowed methods
- `deny_methods` - Specify the denied methods
- `redirect` - temporily (307) or permanantly (301) redirect a request

| Directive            | Type   | Contexts                  | default_values     |
| -------------------- | ------ | ------------------------- | ------------------ |
| http                 | Block  | main                      | N/A                |
| server               | Block  | http                      | N/A                |
| events               | Block  | main                      | worker_connections |
| location             | Block  | server,location           | N/A                |
| types                | Simple | http,server,location      | N/A                |
| listen               | Simple | server                    | *:80 *:8000        |
| root                 | Simple | http,server,location      | html               |
| index                | Simple | http,server,location      | index.html         |
| autoindex            | Simple | http,server,location      | off                |
| error_page           | Simple | http,server,location      | N/A                |
| access_log           | Simple | http,server,location      | logs/access.log    |
| error_log            | Simple | main,http,server,location | logs/error.log     |
| server_name          | Simple | server                    | ""                 |
| client_max_body_size | Simple | http,server,location      | 1m                 |
| include              | Simple | main,http,server,location | N/A                |
| cgi                  | Simple | http,server,location      | N/A                |
| allow_methods        | Simple | http,server,location      | GET POST           |
| deny_methods         | Simple | http,server,location      | DELETE             |
| redirect             | Simple | server,location           | N/A                |

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