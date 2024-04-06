# POST Request
## Simple POST Request
### Information contained

Start line: method, path/to/resources and the HTTP version

Entity headers: Content-Type, Content-Length
General headers: User-Agent, Referer
Request headers: Accept

Entity body exists

### Success Response

- 200 OK
- 201 Created

- Fields: Content-Type, Content-Length(when the response body has a fixed length), Date, Server, Allow(methods allowed for the requested resource)
- For 201: Location (the URL of the newly created resource)

### Error Response

- 400 Bad Request

- 403 Forbidden
- 404 Not Found
- 408 Request Timeout

- 422 Unprocessable Entity : validation errors, data integrity issues

...?

- 500 Internal Server Error
- 503 Service Unavailable

- Fields: Content-Type, Content-Length, Date, Server, Connection ... ?
- For 408: Retry-After

## CGI POST Request