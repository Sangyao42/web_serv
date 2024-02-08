# Web Server

> A HTTP server written in C++ from scratch

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
        }
        location /cgi-bin {
            allow GET;
            allow POST;
        }
        redirect /old /new;
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
- `redirect` - The redirect block of the server
