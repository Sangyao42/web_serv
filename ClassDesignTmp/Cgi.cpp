#include "Client.hpp"

#include <sys/wait.h>
#include <cassert>

void	cgi::ProcessGetRequestCgi(struct Client *clt)
{
	int	cgi_input[2], cgi_output[2];
	int pipes = SetPipes(cgi_input, cgi_output, clt->req->getMethod()); //For CGI GET request, only cgi_output[2] is needed
	if (pipes < 0)
	{
		clt->status_code = k500;
		return (res_builder::GenerateErrorResponse(clt));
	}
	int pid = fork();
	if (pid < 0)
	{
		close(cgi_output[PipeEnd::kRead]);
		close(cgi_output[PipeEnd::kWrite]);
		clt->status_code = k500;
		return (res_builder::GenerateErrorResponse(clt));
	}
	if (pid == 0)
	{
		//child process
		close(cgi_output[PipeEnd::kRead]);
		dup2(cgi_output[PipeEnd::kWrite], STDOUT_FILENO);
		assert(!clt->cgi_argv.empty() && "ProcessGetRequestCgi: clt->cgi_argv is NULL");
		// char* cgi_executable = clt->cgi_executable.c_str(); // get excutable from looping through location->cgis based on the file extension and check permission
		// char* cgi_path = clt->path.c_str(); // get excutable from looping through location->cgis based on the file extension and check permission
		if (access(clt->cgi_argv[0].c_str(), F_OK | X_OK) != 0 || \
		access(clt->cgi_argv[1].c_str(), F_OK | R_OK) != 0)
		{
			close(cgi_output[PipeEnd::kWrite]);
			exit(1);
		}
		// std::vector<char*> cgi_argv = ConstructExecArray(clt->cgi_argv);
		char** cgi_argv = StringVecToTwoDimArray(clt->cgi_argv);
		SetCgiEnv(clt);
		// std::vector<char*> cgi_env = ConstructExecArray(clt->cgi_env);
		char** cgi_env = StringVecToTwoDimArray(clt->cgi_env);
		if (cgi_argv == NULL || cgi_env == NULL)
		{
			close(cgi_output[PipeEnd::kWrite]);
			exit(1);
		}
		execve(cgi_argv[0], cgi_argv, cgi_env);
		// int checkfree = FreeTwoDimArray(cgi_argv);
		// int checkfree2 = FreeTwoDimArray(cgi_env);
		std::cerr << "Error: execve" << std::endl;
		close(cgi_output[PipeEnd::kWrite]);
		exit(1);
	}
	//parent process
	std::string response_tmp;
	close(cgi_output[PipeEnd::kWrite]);
	int wstats;
	waitpid(pid, &wstats, 0);
	if (WIFEXITED(wstats) && WEXITSTATUS(wstats) == 0)
	{
		int read_byte = ReadAll(cgi_output[PipeEnd::kRead], response_tmp);
		close(cgi_output[PipeEnd::kRead]);
		assert(read_byte != 0 && "ReadAll: read byte is 0");
		if (read < 0)
		{
			clt->status_code = k500;
			return (res_builder::GenerateErrorResponse(clt));
		}
		ParseResponseTmp(response_tmp); // modify the clt->response by the received message from cgi
		std::string res_content_type = response_tmp.substr(response_tmp.find("Content-Type: "),response_tmp.find("\r\n\r\n")); // TODO: get the content type from the string returned by cgi, in std::string or a vector of string
		std::string res_body;
		std::string res_length = res_body.size();
		if (!IsSupportedMediaType(res_content_type, clt->config->query->mime_types)) //check the response content type with the MIME type
		{
			clt->status_code = k500;
			return (res_builder::GenerateErrorResponse(clt));
		}
		if (IsAcceptable(res_content_type)) // TODO: get the content type from the string returned by cgi, check Accept header and MIME type && check response entity's content type and Accept Header
		{
			clt->status_code = k406;
			return (res_builder::GenerateErrorResponse(clt));
		}
		else
		{
			// TODO: generate the response body with content type and content length
			clt->cgi_content_type = res_content_type;
			clt->cgi_content_length = res_length;
			clt->res->setResponseBody(res_body);
			clt->status_code = k200;
			return (res_builder::GenerateSuccessResponse(clt));
		}
	}
	else
	{
		clt->status_code = k500;
		return (res_builder::GenerateErrorResponse(clt));
	}
}

void	cgi::ProcessPostRequestCgi(struct Client *clt)
{
	int	cgi_input[2], cgi_output[2];
	int pipes = SetPipes(cgi_input, cgi_output, clt->req->getMethod()); //For CGI GET request, only cgi_output[2] is needed
	if (pipes < 0)
	{
		clt->status_code = k500;
		return (res_builder::GenerateErrorResponse(clt));
	}
	int pid = fork();
	if (pid < 0)
	{
		close(cgi_input[PipeEnd::kRead]);
		close(cgi_input[PipeEnd::kWrite]);
		close(cgi_output[PipeEnd::kRead]);
		close(cgi_output[PipeEnd::kWrite]);
		clt->status_code = k500;
		return (res_builder::GenerateErrorResponse(clt));
	}
	if (pid == 0)
	{
		//child process
		close(cgi_output[PipeEnd::kRead]);
		dup2(cgi_output[PipeEnd::kWrite], STDOUT_FILENO);
		close(cgi_input[PipeEnd::kWrite]);
		dup2(cgi_input[PipeEnd::kRead], STDIN_FILENO);
		if (access(clt->cgi_argv[0].c_str(), F_OK | X_OK) != 0 || \
		access(clt->cgi_argv[1].c_str(), F_OK | R_OK) != 0)
		{
			close(cgi_input[PipeEnd::kRead]);
			close(cgi_output[PipeEnd::kWrite]);
			exit(1);
		}
		// std::vector<char*> cgi_argv = ConstructExecArray(clt->cgi_argv);
		char** cgi_argv = StringVecToTwoDimArray(clt->cgi_argv);
		SetCgiEnv(clt);
		// std::vector<char*> cgi_env = ConstructExecArray(clt->cgi_env);
		char** cgi_env = StringVecToTwoDimArray(clt->cgi_env);
		if (cgi_argv == NULL || cgi_env == NULL)
		{
			close(cgi_input[PipeEnd::kRead]);
			close(cgi_output[PipeEnd::kWrite]);
			exit(1);
		}
		execve(cgi_argv[0], cgi_argv, cgi_env);
		// int checkfree = FreeTwoDimArray(cgi_argv);
		// int checkfree2 = FreeTwoDimArray(cgi_env);
		std::cerr << "Error: execve" << std::endl;
		close(cgi_input[PipeEnd::kRead]);
		close(cgi_output[PipeEnd::kWrite]);
		exit(1);
	}
	//parent process
	//write to child process
	close(cgi_input[PipeEnd::kRead]);
	int content_size = clt->req->getRequestBody().size();
	std::string content = clt->req->getRequestBody();
	char *content_str = const_cast<char *>(content.c_str());
	int write_byte = WriteAll(cgi_input[PipeEnd::kWrite], content_str, content_size);
	assert (write_byte != 0 && "WriteAll: write byte is 0");
	if (write_byte < 0 && write_byte != content_size)
	{
		close(cgi_input[PipeEnd::kWrite]);
		close(cgi_output[PipeEnd::kRead]);
		close(cgi_output[PipeEnd::kWrite]);
		clt->status_code = k500;
		return (res_builder::GenerateErrorResponse(clt));
	}
	close(cgi_input[PipeEnd::kWrite]);

	//read from child process
	std::string response_tmp;
	close(cgi_output[PipeEnd::kWrite]);
	int wstats;
	waitpid(pid, &wstats, 0);
	if (WIFEXITED(wstats) && WEXITSTATUS(wstats) == 0)
	{
		int read_byte = ReadAll(cgi_output[PipeEnd::kRead], response_tmp);
		close(cgi_output[PipeEnd::kRead]);
		assert(read_byte != 0 && "ReadAll: read byte is 0");
		if (read < 0)
		{
			clt->status_code = k500;
			return (res_builder::GenerateErrorResponse(clt));
		}
		ParseResponseTmp(); // modify the clt->response by the received message from cgi
		std::string res_content_type = response_tmp.substr(response_tmp.find("Content-Type: "),response_tmp.find("\r\n\r\n")); // TODO: get the content type from the string returned by cgi
		std::string res_body;
		std::string res_length = res_body.size();
		if (!IsSupportedMediaType(res_content_type, clt->config->query->mime_types)) //check the response content type with the MIME type
		{
			clt->status_code = k500;
			return (res_builder::GenerateErrorResponse(clt));
		}
		if (IsAcceptable(res_content_type)) // TODO: get the content type from the string returned by cgi, check Accept header and MIME type && check response entity's content type and Accept Header
		{
			clt->status_code = k406;
			return (res_builder::GenerateErrorResponse(clt));
		}
		else
		{
			// TODO: generate the response body with content type and content length
			clt->cgi_content_type = res_content_type;
			clt->cgi_content_length = res_length;
			clt->res->setResponseBody(res_body);
			clt->status_code = k200;
			return (res_builder::GenerateSuccessResponse(clt));
		}
	}
	else
	{
		clt->status_code = k500;
		return (res_builder::GenerateErrorResponse(clt));
	}

}

//cgi related functions
int cgi::SetPipes(int *cgi_input, int *cgi_output, const Method method)
{
	assert((method == kGet || method == kPost) && "SetPipes: method is not GET or POST");
	if (pipe(cgi_output) < 0)
		return (-1);
	if (method == kPost)
	{
		if (pipe(cgi_input) < 0)
		{
			close(cgi_output[PipeEnd::kRead]);
			close(cgi_output[PipeEnd::kWrite]);
			return (-1);
		}
	}
}

#define stringify(name) #name

void	cgi::SetCgiEnv(struct Client *clt)
{
	//env: method, query_string, content_length, content_type,
	//request_uri, document_uri, document_root, script_name,
	//server_addr, server_port, server_protocol, server_name

	//construct method
	// char* convert_enum_to_string[] = {stringify(Monday), stringify(Tuesday), stringify(Wednesday), stringify(Thursday)};
	// std::string method = convert_enum_to_string[clt->req->getMethod()];
	// clt->cgi_env.push_back("REQUEST_METHOD=" + method);
	clt->cgi_env.push_back("REQUEST_METHOD=" + clt->req->getMethod());

	//construct query_string
	uri::Query query = clt->req->getRequestTarget().query;
	std::string query_string;
	for (size_t i = 0; i < query.size(); ++i)
	{
		if (i == query.size() - 1)
			query_string += query[i].first + "=" + query[i].second;
		else
			query_string += query[i].first + "=" + query[i].second + "&";
	}
	clt->cgi_env.push_back("QUERY_STRING=" + query_string);

	//construct content_length
	HeaderInt *content_length = dynamic_cast<HeaderInt *>(clt->req->returnValueAsPointer("Content-Length"));
	std::ostringstream content_length_str;
	content_length->toStringStream(content_length_str);
	if (content_length && content_length_str.good())
		clt->cgi_env.push_back("CONTENT_LENGTH=" + content_length_str.str());
	else
		clt->cgi_env.push_back("CONTENT_LENGTH=");

	//construct content_type //TODO: get content type from the request header as HeaderString or vector of string
	HeaderString *content_type = dynamic_cast<HeaderString *>(clt->req->returnValueAsPointer("Content-Type"));
	if (content_type)
		clt->cgi_env.push_back("CONTENT_TYPE=" + content_type->content());
	else
		clt->cgi_env.push_back("CONTENT_TYPE=");

	//construct request_uri
	clt->cgi_env.push_back("REQUEST_URI=" + clt->req->getRequestTarget().path);

	//construct document_uri // ? what is this? Probably for POST request with Location header ?

	//construct document_root
	std::string document_root = clt->config->query->root;
	clt->cgi_env.push_back("DOCUMENT_ROOT=" + document_root); //? without the prefix . ?

	//construct script_name
	size_t pos = clt->path.find(clt->config->query->root) + clt->config->query->root.size();
	std::string script_name = clt->path.substr(pos);
	clt->cgi_env.push_back("SCRIPT_NAME=" + script_name);

	//construct scipt_filename
	clt->cgi_env.push_back("SCRIPT_FILENAME=" + document_root + script_name);

}
// fastcgi_param  SCRIPT_FILENAME    $document_root$fastcgi_script_name;
// fastcgi_param  QUERY_STRING       $query_string;
// fastcgi_param  REQUEST_METHOD     $request_method;
// fastcgi_param  CONTENT_TYPE       $content_type;
// fastcgi_param  CONTENT_LENGTH     $content_length;d
// fastcgi_param  SCRIPT_NAME        $fastcgi_script_name;
// fastcgi_param  REQUEST_URI        $request_uri;
// fastcgi_param  DOCUMENT_URI       $document_uri;
// fastcgi_param  DOCUMENT_ROOT      $document_root;
// fastcgi_param  SERVER_PROTOCOL    $server_protocol;
// fastcgi_param  GATEWAY_INTERFACE  CGI/1.1;
// fastcgi_param  SERVER_SOFTWARE    nginx/$nginx_version;
// fastcgi_param  REMOTE_ADDR        $remote_addr;
// fastcgi_param  REMOTE_PORT        $remote_port;
// fastcgi_param  SERVER_ADDR        $server_addr;
// fastcgi_param  SERVER_PORT        $server_port;
// fastcgi_param  SERVER_NAME        $server_name;

// fastcgi_index  index.php;

// fastcgi_param  REDIRECT_STATUS    200;

// std::vector<char *>	process::ConstructExecArray(std::vector<std::string> &cgi_params) //{extension, cgi_path, NULL}
// {
// 	std::vector<char*> cstrings;
// 	int cstrs_size = StringVecToTwoDimArray(cstrings, cgi_params);
// 	if (cstrs_size == 1)
// 	{
// 		std::cerr << "Error: ConstructExecveArrays" << std::endl;
// 		return (std::vector<char*>());
// 	}
// 	return cstrings;
// }

int cgi::ReadAll(int fd, std::string &response_tmp)
{
	char buffer[1024];
	int read_byte = 0;
	int total_read = 0;
	while ((read_byte = read(fd, buffer, 1024)) > 0)
	{
		response_tmp.append(buffer, read_byte);
		total_read += read_byte;
	}
	if (read_byte < 0)
	{
		std::cerr << "Error: ReadAll from child process" << std::endl;
		return (-1);
	}
	return (total_read);
}

int cgi::WriteAll(int fd, char *cstr_buf, int size)
{
	int total_write = 0;
	int byte_left = size;
	int write_byte = 0;
	while (total_write < size)
	{
		write_byte = write(fd, cstr_buf + total_write, byte_left);
		if (write_byte < 0)
		{
			std::cerr << "Error: WriteAll to child process" << std::endl;
			return (-1);
		}
		total_write += write_byte;
		byte_left -= write_byte;
	}
	return (total_write);

}

//helper functions
// int	process::StringVecToTwoDimArray(std::vector<char *> &cstrings,const std::vector<std::string> &strings)
// {
// 	size_t vector_size = strings.size();
// 	cstrings.reserve(vector_size + 1);
// 	for(size_t i = 0; i < vector_size; ++i)
// 		cstrings.push_back(const_cast<char*>(strings[i].c_str()));
// 	cstrings.push_back(NULL);
// 	return cstrings.size();
// }

char**	cgi::StringVecToTwoDimArray(const std::vector<std::string> &strings)
{
	std::vector<char *> cstrings;
	size_t vector_size = strings.size();
	cstrings.reserve(vector_size + 1);
	for(size_t i = 0; i < vector_size; ++i)
		cstrings.push_back(const_cast<char*>(strings[i].c_str()));
	cstrings.push_back(NULL);
	if (cstrings.size() == 1)
		return NULL;
	return &cstrings[0];
}
