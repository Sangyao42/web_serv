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
- [kqueue](https://habr.com/en/articles/600123/) (BSD or mac)

## cgi

cgi program is a script that when executed, will produce a html response on the stdout.

www.hi.com/hi.php

## Configuration file

The configuration file is a nginx styled file. It is used to configure the server. The file is read at the start of the server and the server will use the configuration specified in the file.

Supported configuration options:

| Directive            | Type   | Contexts                  | Default values     | On repeat | order matters | arguments              |
| -------------------- | ------ | ------------------------- | ------------------ | --------- | ------------- | ---------------------- |
| http                 | Block  | main                      | N/A                | error     | appear once   | {}                     |
| server               | Block  | http                      | N/A                | append    | yes           | {}                     |
| events               | Block  | main                      | worker_connections | error     | appear once   | {}                     |
| location             | Block  | server,location           | N/A                | append    | yes           | {}                     |
| listen               | Simple | server                    | *:80 *:8000        | append    | no            | ip_address*            |
| server_name          | Simple | server                    | ""                 | append    | no            | word*                  |
| allow_methods        | Simple | http,server,location      | GET POST           | append    | yes           | (GET \| POST \| DELETE)* |
| root                 | Simple | http,server,location      | html               | overwrite | yes           | path                   |
| index                | Simple | http,server,location      | index.html         | append    | yes           | path*                  |
| types                | Simple | http,server,location      | N/A                | overwrite | yes           | { text/plain * }       |
| error_page           | Simple | http,server,location      | N/A                | append    | yes           | path                   |
| client_max_body_size | Simple | http,server,location      | 1m                 | overwrite | yes           | number unit            |
| redirect             | Simple | server,location           | N/A                | overwrite | yes           | path (redirect \| permanent) |
| autoindex            | Simple | http,server,location      | off                | overwrite | yes           | on \| off              |
| cgi                  | Simple | http,server,location      | N/A                | append    | yes           | word path              |
| access_log           | Simple | http,server,location      | logs/access.log    | overwrite | yes           | path                   |
| error_log            | Simple | main,http,server,location | logs/error.log     | overwrite | yes           | path                   |
| include              | Simple | main,http,server,location | N/A                | N/A       | yes           | path                   |
| worker_connections   | Simple | events                    | 512                | overwrite | appear once   | number                 |

### On repeat

append mode - When the directive is used, all the repeated directives of the same type (from the current context to the main context) are queried, the most matching entry will be used.

overwrite mode - Only the latest directive is used.

error mode - Repeated directive is not allowed.

### Order

Need multimap to perserve multiple occurance of the same type of directives.
The order of directives are recorded in every directive.

```conf
index index.html;

location /hello {}; inside this block, index is index.html

index hello.htm;

location / {}; inside this block, index is hello.htm
```

### Functionality

| Directive            | status code | Functionality                             
| -------------------- | ----------- | ------------------------------------------
| http                 |             | search_server, get_all_ip_addresses
| server               |             | search_location
| listen               |             | search_server, get_all_ip_addresses
| location             |             | 
| server_name          |             | search_server
| allow_methods        | 405         | request_is_accepted
| root                 | 403,404     | construct_full_path
| index                | 403,404     | construct_full_path
| types                |             | get_response_content_type
| error_page           | all         | generate_templated_response
| client_max_body_size |             | request_is_accepted, get_request_max_size
| redirect             | 301,307     | generate_header_response
| autoindex            |             | construct_full_path, generate_templated_response
| cgi                  |             | generate_cgi_response
| access_log           |             | log_response
| error_log            |             | log_response
| include              |             |
|
| events               |             | get_worker_connections
| worker_connections   |             | get_worker_connections

- `index` has to match all the entries to find the best match.
- `search` has to return a path to the best match. The path is used to decide which directive is in relevant to the current path match.
