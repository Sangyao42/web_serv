#include "Client.hpp"

#include <sys/wait.h>
#include <cassert>
#include <stdlib.h>

void	cgi::ProcessGetRequestCgi(struct Client *clt)
{
	int	cgi_input[2], cgi_output[2];
	int pipes = SetPipes(cgi_input, cgi_output, clt->req.getMethod()); //For CGI GET request, only cgi_output[2] is needed
	if (pipes < 0)
	{
		clt->status_code = k500;
		return (res_builder::GenerateErrorResponse(clt));
	}
	int pid = fork();
	if (pid < 0)
	{
		close(cgi_output[kRead]);
		close(cgi_output[kWrite]);
		clt->status_code = k500;
		return (res_builder::GenerateErrorResponse(clt));
	}
	if (pid == 0)
	{
		//child process
		close(cgi_output[kRead]);
		dup2(cgi_output[kWrite], STDOUT_FILENO);
		assert(!clt->cgi_argv.empty() && "ProcessGetRequestCgi: clt->cgi_argv is NULL");
		if (access(clt->cgi_argv[0].c_str(), F_OK | X_OK) != 0 || \
		access(clt->cgi_argv[1].c_str(), F_OK | R_OK) != 0)
		{
			close(cgi_output[kWrite]);
			exit(2);
		}
		SetCgiEnv(clt);
		std::vector<char *> cstrings_argv;
		std::vector<char *> cstrings_env;
		char** cgi_argv = StringVecToTwoDimArray(cstrings_argv, clt->cgi_argv);
		char** cgi_env = StringVecToTwoDimArray(cstrings_env, clt->cgi_env);
		if (cgi_argv == NULL || cgi_env == NULL)
		{
			close(cgi_output[kWrite]);
			exit(3);
		}
		execve(cgi_argv[0], cgi_argv, cgi_env);
		std::cerr << "Error: execve" << std::endl;
		close(cgi_output[kWrite]);
		exit(4);
	}
	//parent process48
	std::string response_tmp;
	close(cgi_output[kWrite]);
	int wstats;
	waitpid(pid, &wstats, 0);
	if (WIFEXITED(wstats) && (WEXITSTATUS(wstats) == 0))
	{
		int read_byte = ReadAll(cgi_output[kRead], response_tmp);
		close(cgi_output[kRead]);
		assert(read_byte != -1 && "ReadAll: read byte is -1");
		if (read_byte < 0)
		{
			clt->status_code = k500;
			return (res_builder::GenerateErrorResponse(clt));
		}
		struct CgiOutput cgi_content;
		if(ParseCgiOutput(cgi_content, response_tmp) == false)
		{
			clt->status_code = k500;
			return (res_builder::GenerateErrorResponse(clt));
		}
		if (cgi_content.content_type.empty() && cgi_content.content_body.empty())
		{
			clt->status_code = k204;
			return (res_builder::GenerateSuccessResponse(clt));
		}
		if (!process:: IsSupportedMediaType(cgi_content.content_type, clt->config.query->mime_types)) //check the response content type with the MIME type
		{
			clt->status_code = k500;
			return (res_builder::GenerateErrorResponse(clt));
		}
		// if (IsAcceptable(cgi_output.content_type)) // TODO: get the content type from the string returned by cgi, check Accept header and MIME type && check response entity's content type and Accept Header
		// {
		// 	clt->status_code = k406;
		// 	return (res_builder::GenerateErrorResponse(clt));
		// }
		else
		{
			// TODO: generate the response body with content type and content length
			clt->cgi_content_type = cgi_content.content_type;
			clt->cgi_content_length = cgi_content.content_body.size();
			clt->res.setResponseBody(cgi_content.content_body);
			clt->status_code = k200;
			return (res_builder::GenerateSuccessResponse(clt));
		}
	}
	else
	{
		close(cgi_output[kRead]);
		clt->status_code = k500;
		return (res_builder::GenerateErrorResponse(clt));
	}
}

void	cgi::ProcessPostRequestCgi(struct Client *clt)
{
	int	cgi_input[2], cgi_output[2];
	int pipes = SetPipes(cgi_input, cgi_output, clt->req.getMethod()); //For CGI GET request, only cgi_output[2] is needed
	if (pipes < 0)
	{
		clt->status_code = k500;
		return (res_builder::GenerateErrorResponse(clt));
	}
	int pid = fork();
	if (pid < 0)
	{
		close(cgi_input[kRead]);
		close(cgi_input[kWrite]);
		close(cgi_output[kRead]);
		close(cgi_output[kWrite]);
		clt->status_code = k500;
		return (res_builder::GenerateErrorResponse(clt));
	}
	if (pid == 0)
	{
		//child process
		close(cgi_output[kRead]);
		dup2(cgi_output[kWrite], STDOUT_FILENO);
		close(cgi_input[kWrite]);
		dup2(cgi_input[kRead], STDIN_FILENO);
		if (access(clt->cgi_argv[0].c_str(), F_OK | X_OK) != 0 || \
		access(clt->cgi_argv[1].c_str(), F_OK | R_OK) != 0)
		{
			close(cgi_input[kRead]);
			close(cgi_output[kWrite]);
			exit(1);
		}
		SetCgiEnv(clt);
		std::vector<char *> cstrings_argv;
		std::vector<char *> cstrings_env;
		char** cgi_argv = StringVecToTwoDimArray(cstrings_argv, clt->cgi_argv);
		char** cgi_env = StringVecToTwoDimArray(cstrings_env, clt->cgi_env);
		if (cgi_argv == NULL || cgi_env == NULL)
		{
			close(cgi_input[kRead]);
			close(cgi_output[kWrite]);
			exit(1);
		}
		execve(cgi_argv[0], cgi_argv, cgi_env);
		std::cerr << "Error: execve" << std::endl;
		close(cgi_input[kRead]);
		close(cgi_output[kWrite]);
		exit(1);
	}
	//parent process
	//write to child process
	close(cgi_input[kRead]);
	int content_size = clt->req.getRequestBody().size();
	char *content_str = const_cast<char *>(clt->req.getRequestBody().c_str());
	int write_byte = WriteAll(cgi_input[kWrite], content_str, content_size);
	assert (write_byte != -1  && "WriteAll: write() failed.");
	if (write_byte < 0 && write_byte != content_size)
	{
		close(cgi_input[kWrite]);
		close(cgi_output[kRead]);
		close(cgi_output[kWrite]);
		clt->status_code = k500;
		return (res_builder::GenerateErrorResponse(clt));
	}
	close(cgi_input[kWrite]);

	//read from child process
	std::string response_tmp;
	close(cgi_output[kWrite]);
	int wstats;
	waitpid(pid, &wstats, 0);
	if (WIFEXITED(wstats) && WEXITSTATUS(wstats) == 0)
	{
		int read_byte = ReadAll(cgi_output[kRead], response_tmp);
		close(cgi_output[kRead]);
		assert(read_byte != 0 && "ReadAll: read byte is 0");
		if (read_byte < 0)
		{
			clt->status_code = k500;
			return (res_builder::GenerateErrorResponse(clt));
		}
		struct CgiOutput cgi_output;
		if(ParseCgiOutput(cgi_output, response_tmp) == false)
		{
			clt->status_code = k500;
			return (res_builder::GenerateErrorResponse(clt));
		}
		if (cgi_output.content_type.empty() && cgi_output.content_body.empty())
		{
			clt->status_code = k204;
			return (res_builder::GenerateSuccessResponse(clt));
		}
		if (!process::IsSupportedMediaType(cgi_output.content_type, clt->config.query->mime_types)) //check the response content type with the MIME type
		{
			clt->status_code = k500;
			return (res_builder::GenerateErrorResponse(clt));
		}
		// if (IsAcceptable(cgi_output.content_type)) // TODO: get the content type from the string returned by cgi, check Accept header and MIME type && check response entity's content type and Accept Header
		// {
		// 	clt->status_code = k406;
		// 	return (res_builder::GenerateErrorResponse(clt));
		// }
		else
		{
			// TODO: generate the response body with content type and content length
			clt->cgi_content_type = cgi_output.content_type;
			clt->cgi_content_length = cgi_output.content_body.size();
			clt->res.setResponseBody(cgi_output.content_body);
			clt->status_code = k200;
			return (res_builder::GenerateSuccessResponse(clt));
		}
	}
	else
	{

		close(cgi_output[kRead]);
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
			close(cgi_output[kRead]);
			close(cgi_output[kWrite]);
			return (-1);
		}
	}
	return (0);
}

#define stringify(name) #name

void	cgi::SetCgiEnv(struct Client *clt)
{
	//env: method, query_string, content_length, content_type,
	//request_uri, document_uri, document_root, script_name,
	//server_addr, server_port, server_protocol, server_name

	//construct method
	std::string method;
	switch (clt->req.getMethod())
	{
		case kGet: method = "GET"; break;
		case kPost: method = "POST"; break;
		case kDelete: method = "DELETE"; break;
		default: {}
	};
	clt->cgi_env.push_back("REQUEST_METHOD=" + method);
	// clt->cgi_env.push_back("REQUEST_METHOD=" + clt->req.getMethod());

	//construct query_string
	clt->cgi_env.push_back("QUERY_STRING=" + clt->req.getRequestTarget().query);

	//construct content_length
	HeaderInt *content_length = static_cast<HeaderInt *>(clt->req.returnValueAsPointer("Content-Length"));
	if (content_length)
	{
		std::string content_length_str = content_length->to_string();
		clt->cgi_env.push_back("CONTENT_LENGTH=" + content_length_str);
	}
	else
		clt->cgi_env.push_back("CONTENT_LENGTH=");

	//construct content_type
	HeaderString *content_type = static_cast<HeaderString *>(clt->req.returnValueAsPointer("Content-Type"));
	if (content_type)
		clt->cgi_env.push_back("CONTENT_TYPE=" + content_type->content());
	else
		clt->cgi_env.push_back("CONTENT_TYPE=");

	//construct request_uri
	clt->cgi_env.push_back("REQUEST_URI=" + clt->req.getRequestTarget().path);

	//construct document_uri // ? what is this? Probably for POST request with Location header ?

	//construct document_root
	std::string document_root = clt->config.query->root;
	// clt->cgi_env.push_back("DOCUMENT_ROOT=.." + document_root);
	clt->cgi_env.push_back("DOCUMENT_ROOT=" + document_root);

	//construct script_name
	size_t pos = clt->path.find(clt->config.query->root) + clt->config.query->root.size();
	std::string script_name = clt->path.substr(pos);
	clt->cgi_env.push_back("SCRIPT_NAME=" + script_name);

	//construct scipt_filename
	// clt->cgi_env.push_back("SCRIPT_FILENAME=.." + document_root + script_name);
	clt->cgi_env.push_back("SCRIPT_FILENAME=" + document_root + script_name);
}

/**
 * nginx example for fastcgi_param
   fastcgi_param  SCRIPT_FILENAME    $document_root$fastcgi_script_name;
   fastcgi_param  QUERY_STRING       $query_string;
   fastcgi_param  REQUEST_METHOD     $request_method;
   fastcgi_param  CONTENT_TYPE       $content_type;
   fastcgi_param  CONTENT_LENGTH     $content_length;d
   fastcgi_param  SCRIPT_NAME        $fastcgi_script_name;
   fastcgi_param  REQUEST_URI        $request_uri;
   fastcgi_param  DOCUMENT_URI       $document_uri;
   fastcgi_param  DOCUMENT_ROOT      $document_root;
   fastcgi_param  SERVER_PROTOCOL    $server_protocol;
   fastcgi_param  GATEWAY_INTERFACE  CGI/1.1;
   fastcgi_param  SERVER_SOFTWARE    nginx/$nginx_version;
   fastcgi_param  REMOTE_ADDR        $remote_addr;
   fastcgi_param  REMOTE_PORT        $remote_port;
   fastcgi_param  SERVER_ADDR        $server_addr;
   fastcgi_param  SERVER_PORT        $server_port;
   fastcgi_param  SERVER_NAME        $server_name;

    fastcgi_index  index.php;
    fastcgi_param  REDIRECT_STATUS    200;
*/



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

bool	cgi::ParseCgiOutput(struct CgiOutput &cgi_content, std::string &response_tmp)
{
	std::string delimiter = "\r\n\r\n";
	size_t pos_delim = response_tmp.find(delimiter);
	size_t pos_cont_type = response_tmp.find("Content-Type: ");
	if (pos_delim == std::string::npos || pos_cont_type == std::string::npos)
	{
		std::cerr << "Error: ParseCgiOutput" << std::endl;
		return false;
	}
	size_t pos = pos_cont_type + sizeof("Content-Type: ") - 1;
 	cgi_content.content_type = response_tmp.substr(pos, pos_delim - pos);
	cgi_content.content_body = response_tmp.substr(pos_delim + delimiter.size());
	if ((cgi_content.content_type.empty() && !cgi_content.content_body.empty()) \
		|| (!cgi_content.content_type.empty() && cgi_content.content_body.empty()))
	{
		std::cerr << "Error: ParseCgiOutput" << std::endl;
		return false;
	}
	return true;
}

//helper functions

char**	cgi::StringVecToTwoDimArray(std::vector<char *> &cstrings, const std::vector<std::string> &strings)
{
	size_t vector_size = strings.size();
	cstrings.reserve(vector_size + 1);
	for(size_t i = 0; i < vector_size; ++i)
		cstrings.push_back(const_cast<char*>(strings[i].c_str()));
	cstrings.push_back(NULL);
	if (cstrings.size() == 1)
		return NULL;
	return &cstrings[0];
}
