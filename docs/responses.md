# For each method
## Get Request (No Authentification)
### Information contained

Start line: method, path/to/resources and the HTTP version

Important Fields: Host, Connection
Only Informational: User-Agent, Referer, Date, Accept ...

### Success Response

1) No Redirection

- GET 200 OK
- Fields: Content-Type, Content-Length, Date, Server, Connection
- For the Cache: Last-Modified

2) Redirection

- 301 Moved Permanently
- 307 Temporary Redirection

- Fields: Location, Date, Server, Connection

### Error Response

1) Some errors handled by the client-side like timeouts

2) Some errors handled by us (?)
- 404 Not Found : incorrect URL
- 500 Internal Server Error : misconfiguration, programming error ...
- 503 Service Unavailable : server overload

* Server-side timeout status code can be 408, 500 or 503

** At which stages these errors occur and which fields are needed for the response