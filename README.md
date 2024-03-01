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