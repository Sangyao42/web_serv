# POST Request
## Simple POST Request
### Information contained

	Start line: method, path/to/resources and the HTTP version

	Entity headers: Content-Type, Content-Length
	General headers: Transfer-Encoding: chunked, User-Agent, Referer, Date
	Request headers: Host, Accept

	Entity body exists

### Success Response

- 200 OK
- 201 Created
- 204 No Content

	Fields: Content-Type, Content-Length(when the response body has a fixed length),
	Date, Server, Allow(methods allowed for the requested resource)

	For 200: Content-Location(if exists, the URL related to entity body)
	For 201: Content-Location (the URL of the newly created resource)

### Error Response

> Parsing
> - 400 Bad Request
> - 413 Request Entity Too Large
> - 414 URI Too Long

> - 403 Forbidden
> - 404 Not Found
> - 405 Method Not Allowed
> - 408 Request Timeout
> - 415 Unsupported Media Type
> - 422 Unprocessable Entity : validation errors, data integrity issues (optional)

> - 500 Internal Server Error
> - 503 Service Unavailable

Fields: Content-Type, Content-Length, Date, Server

For 405: Allow

For 408: Connection: close

For 415: Accept, Accept-Encoding

** For methods other than GET, POST, DELETE : 501 Not Implemented for Unsupported methods (parsing)

## CGI POST Request
### Request Message

    Request line: Method URI(w./wo. query) HTTP-Version

	Request headers: Host(mandatory), Accept
	General headers: Transfer-Encoding: chunked, Date, User-Agent, Referer
	Entity headers: Content-Type, Content-Length

	Entity body


### Success Response

1) No Redirection

    - 200 OK

          Fields: Content-Type, Content-Length, Date, Server

2) Redirection

    - 301 Moved Permanently
    - 307 Temporary Redirection

		  Fields: Content-Type, Content-Length, Location, Date, Server

### Error Response

> Parsing
> - 400 Bad Request
> - 414 Request URI Too Long

> - 400 Bad Request : <b>missing or incorrect query</b>(based on the cgi implementation)
> - 403 Forbidden : <b>upload executable code with cgi extention</b> Log in the error.log file.
> - 404 Not Found
> - 405 Method Not Allowed
> - 406 Not Acceptable
> - 408 Request Timeout

> - 500 Internal Server Error : misconfiguration, programming error, <b>cgi failure</b>

    Fields: Content-Type, Content-Length, Date, Server

    For 405: Allow

    For 408: Connection: close
