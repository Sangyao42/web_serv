# GET Request
## Simple Get Request (No Authentification, No CGI)
### Information contained

    Start line: method, path/to/resources and the HTTP version

    Important Fields: Host

    Only Informational: User-Agent, Referer, Date, Accept ...

** while a GET request typically does not have a body, a response to a GET request has a html body sent back, successful or not

### Success Response

1) No Redirection

    - 200 OK

        Fields: Content-Type, Content-Length, Date, Server

        For the Cache: Last-Modified

2) Redirection

    - 301 Moved Permanently
    - 307 Temporary Redirection

        Fields: Content-Type, Content-Length, Location, Date, Server

		For the Cache: Last-Modified

### Error Response

> - 304 Not Modified : for the tag "If-Modified-Since" (optional)
> - 412 Precondition Failed : for the tag "If-Unmodified-Since" (optional)

> Parsing
> - 400 Bad Request : invalid query parameters, invalid URL (parsing), no Host Header in Request (parsing)
> - 403 Forbidden : Server refuse to fulfill the request. Possible reason: no index in Configuration(parsing), Log in the error.log file.
> - 414 Request URI Too Long (parsing)

> - 403 Forbidden : Server refuse to fulfill the request. Possible reason: no permission. Log in the error.log file.
> - 404 Not Found : incorrect URL
> - 405 Method Not Allowed : method not supported by the target resource
> - 406 Not Acceptable: for request with Accept header, sender cannot send the resources which are acceptable by the reciever
> - 408 Request Timeout : the client failed to produce a request within the server's timeout period

> - 500 Internal Server Error : misconfiguration, programming error ...
> - 503 Service Unavailable : server overload ...

Fields: Content-Type, Content-Length, Date, Server

For 405: Allow

For 408: Connection: close

## CGI GET Request