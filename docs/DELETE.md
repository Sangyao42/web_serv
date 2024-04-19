# DELETE Request
## Simple DELETE Request
### Information contained

    Start line: method, path/to/resources and the HTTP version

    Host, User-Agent, Date ...

### Success Response

- 200 OK : if the action has been enacted and the response message includes a representation describing the status

- 204 No Content : if the action has been enacted and no further information is to be supplied.

    Fields: Date, Server, optionally Content-Type and Content-Length

### Error Response

> Parsing
> - 400 Bad Request
> - 403 Forbidden
> - 414 URI Too Long

> - 403 Forbidden
> - 404 Not Found
> - 405 Method Not Allowed
> - 408 Request Timeout

> - 500 Internal Server Error
> - 503 Service Unavailable

For 405: Allow

For 408: Connection: close

## No CGI Implementation
