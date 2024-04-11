# DELETE Request
## Simple DELETE Request
### Information contained

Start line: method, path/to/resources and the HTTP version

Host, Connection, User-Agent, Date ...

### Success Response

- 200 OK : if the action has been enacted and the response message includes a representation describing the status
   
- 204 No Content : if the action has been enacted and no further information is to be supplied.

Fields:
Date, Server ...
optionally Content-Type, Content-Length

### Error Response

> - 403 Forbidden   
> - 404 Not Found   
> - 405 Method Not Allowed   
> - 408 Request Timeout   
> - 500 Internal Server Error   
> - 503 Service Unavailable   

## No CGI Implementation