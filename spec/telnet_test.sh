#!/usr/bin/expect

spawn telnet localhost 8080
expect "Escape character is '^]'." {
	send "POST /upload/new.html HTTP/1.1\nHost: localhost\nContent-Type: text/html\nTransfer-Encoding: chunked\n\n6\n<html>\n7\n</html>\n00000\n\n"
	send "POST /upload/new.html HTTP/1.1\nHost: localhost\nContent-Type: text/html\nTransfer-Encoding: chunked\n\n7\n<html>\n7\n</html>\n00000\n\n"
}
interact
