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

| Directive            | Type   | Contexts                  | Default values     | On repeat | arguments              |
| -------------------- | ------ | ------------------------- | ------------------ | --------- | ---------------------- |
| http                 | Block  | main                      | N/A                | error     | {}                     |
| server               | Block  | http                      | N/A                | append    | {}                     |
| events               | Block  | main                      | worker_connections | error     | {}                     |
| location             | Block  | server,location           | N/A                | append    | {}                     |
| listen               | Simple | server                    | *:80 *:8000        | append    | ip_address*            |
| server_name          | Simple | server                    | ""                 | append    | word*                  |
| allow_methods        | Simple | http,server,location      | GET POST           | append    | (GET \| POST \| DELETE)* |
| deny_methods         | Simple | http,server,location      | DELETE             | append    | (GET \| POST \| DELETE)* |
| root                 | Simple | http,server,location      | html               | overwrite | path                   |
| index                | Simple | http,server,location      | index.html         | append    | path*                  |
| types                | Simple | http,server,location      | N/A                | overwrite | {}                     |
| error_page           | Simple | http,server,location      | N/A                | append    | number path            |
| client_max_body_size | Simple | http,server,location      | 1m                 | overwrite | number unit            |
| redirect             | Simple | server,location           | N/A                | append    | path (redirect \| permanent) |
| autoindex            | Simple | http,server,location      | off                | overwrite | on \| off              |
| cgi                  | Simple | http,server,location      | N/A                | append    | word path              |
| access_log           | Simple | http,server,location      | logs/access.log    | overwrite | path                   |
| error_log            | Simple | main,http,server,location | logs/error.log     | overwrite | path                   |
| include              | Simple | main,http,server,location | N/A                | N/A       | path                   |
| worker_connections   | Simple | events                    | 512                | overwrite | number                 |

- kqueue: https://habr.com/en/articles/600123/