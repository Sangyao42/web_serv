#!/usr/bin/expect

spawn telnet localhost 8080
expect "Escape character is '^]'." {
	send "POST /upload/new.html HTTP/1.1\nHost: localhost\nContent-Type: text/html\nTransfer-Encoding: chunked\n\n6\n<html>\n7\n</html>\n00000\n\n"
	send "POST /upload/new.html HTTP/1.1\nHost: localhost\nContent-Type: text/html\nTransfer-Encoding: chunked\n\n7\n<html>\n7\n</html>\n00000\n\n"
}
interact

spawn telnet localhost 8088
expect "Escape character is '^]'." {
	send "POST /redirect_google HTTP/1.1\nHost: localhost\nContent-Length: 2\n\n65\n"
	send "DELETE /redirect_from/index.html HTTP/1.1\nHost: localhost\n\n"
	send "GET /redirect_from HTTP/1.1\nHost: localhost\n\n"
}
interact