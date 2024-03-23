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