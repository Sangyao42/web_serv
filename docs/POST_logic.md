<h3>POST logic</h3>

- check content type from the request with MIME types
- check file exist
	- if file exist, check if it is a directory
		- if it is a directory, create a file with timestamp and with the content-type-> extension for the name
		- if not, it is a file, check if it is cgi
			- if it is a cgi, process the request with the cgi
			- if not a cgi, check if the file is writable
				- if it is not writable, return 403
				- if it is writable, modify the file with the body of the request
	- if file does not exist, check if it is a cgi
		- if it is a cgi, return 403
		- if it is not a cgi, check if the uri ends with a slash
			- if end with a slash, create a file with timestamp and with the content-type-> extension for the name within this directory (loop inside to make sure all the prefix directories is created. If any error occurs, return 500)
			- if end with no slash, create the file w/o the extension (loop inside to make sure all the prefix directories is created, If any error occurs, return 500)
