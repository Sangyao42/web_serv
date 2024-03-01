# Web Server

> A HTTP server written in C++ from scratch

# Structure

nginx configuration read by socker manager

request -> parser -> socket manager

socket manager -> create response

## socket manager

- read from appropriate files
- return back the file content or call the cgi program
- handle error (send proper error code)

### multiplexing library

- select
- poll
- epoll (linux)
- kqueue (BSD or mac)

## cgi

cgi program is a script that when executed, will produce a html response on the stdout.

www.hi.com/hi.php

## Configuration file

The configuration file is a nginx styled file. It is used to configure the server. The file is read at the start of the server and the server will use the configuration specified in the file.

Supported configuration options:

### Events block

```nginx
events {
    use poll;
}
```

- kqueue: https://habr.com/en/articles/600123/

### Types block

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
```

### Http block

```nginx
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
            cgi /cgi-bin;
        }
        location /images {
            allow GET;
			redirect /new;
        }
        location /cgi-bin {
            allow GET;
            allow POST;
        }
    }
}
```

- `limit_except` - Allow or deny methods

- `types` - The types block of the server
- `http` - The http block of the server
- `server` - The server block of the server
- `events` - The events block of the server

- `listen` - The port the server will listen on
- `root` - The root directory of the server
- `index` - The index file of the server
- `autoindex` - Specify whether to show directory listings
- `error_page` - The error page of the server
- `access_log` - The access log file of the server
- `error_log` - The error log file of the server
- `location` - The location block of the server
- `server_name` - The server name of the server
- `client_max_body_size` - The maximum body size of the client
- `include` - Include another configuration file
- `cgi` - Specify a cgi script
- `allow` - Specify the allowed methods

## Specification

### HTTP fields

Body fields

- Content-Type
- Content-Length
- Content-Location

Request fields

- Host
- Referer
- User-Agent
- Authorization (optional)
- Accept (optional)

Response fields

- Last-Modified
- Allow
- Location
- Retry-After (maybe for CGI)
- Server
- WWW-Authenticate, Authentication-Info (optional)

## External materials

- [Memory allocation strategies](https://www.gingerbill.org/series/memory-allocation-strategies/)
- [RFC 9112](https://datatracker.ietf.org/doc/html/rfc9112)
- [RFC 9110 HTTP Semantics](https://datatracker.ietf.org/doc/html/rfc9110)

## TODO

1. Understand socket-related unix system calls
2. Understand select, poll and kqueue

3. Agree on the data structure of the http request and response
4. Agree on the data structure of the nginx configuration file
5. CGI

# Collaboration notes

Accepted git commit message tags:

- feat
- fix
- chore
- perf
- refactor
- doc

Code should follow the [google cpp style](https://anthonytsang.notion.site/Google-C-code-guidelines-ce6a361b17a5415bb41ab264f1866e75?pvs=4)