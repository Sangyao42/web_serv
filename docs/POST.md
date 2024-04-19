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
