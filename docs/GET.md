# GET Request
## Simple Get Request (No Authentification, No CGI)
### Information contained

Start line: method, path/to/resources and the HTTP version

Important Fields: Host, Connection
Only Informational: User-Agent, Referer, Date, Accept ...

** while a GET request typically does not have a body, a response to a GET request has a html body sent back, successful or not

### Success Response

1) No Redirection

- 200 OK

- Fields: Content-Type, Content-Length, Date, Server, Connection
- For the Cache: Last-Modified

2) Redirection

- 301 Moved Permanently
- 307 Temporary Redirection

- Fields: Content-Type, Content-Length, Location, Date, Server, Connection

### Error Response

- 400 Bad Request: invalid query parameters, invalid URL

- 403 Forbidden : no permission
- 404 Not Found : incorrect URL
- 408 Request Timeout : the client failed to produce a request within the server's timeout period

- 500 Internal Server Error : misconfiguration, programming error ...
- 503 Service Unavailable : server overload ...

- Fields: Content-Type, Content-Length, Date, Server, Connection
- For 408: Retry-After

## CGI GET Request