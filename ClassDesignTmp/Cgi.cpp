#include "Client.hpp"

#include <sys/wait.h>
#include <cassert>

void	process::ProcessGetRequestCgi(struct Client *clt)
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
		assert(!clt->cgi_argv.empty() && "ProcessGetRequestCgi: clt->cgi_executable is NULL");
		// char* cgi_executable = clt->cgi_executable.c_str(); // get excutable from looping through location->cgis based on the file extension and check permission
		// char* cgi_path = clt->path.c_str(); // get excutable from looping through location->cgis based on the file extension and check permission
		if (access(clt->cgi_argv[0].c_str(), F_OK | X_OK) != 0 || \
		access(clt->cgi_argv[1].c_str(), F_OK | R_OK) != 0)
		{
			close(cgi_output[PipeEnd::kWrite]);
			exit(1);
		}
		std::vector<char*> cgi_argv = ConstructExecArray(clt->cgi_argv);
		SetCgiEnv(clt);
		std::vector<char*> cgi_env = ConstructExecArray(clt->cgi_env);
		if (cgi_argv.empty() || cgi_env.empty())
		{
			close(cgi_output[PipeEnd::kWrite]);
			exit(1);
		}
		execve(cgi_argv.data()[0], cgi_argv.data(), cgi_env.data());
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
		int read = process::ReadAll(cgi_output[PipeEnd::kRead], response_tmp);
		close(cgi_output[PipeEnd::kRead]);
		if (read < 0)
		{
			clt->status_code = k500;
			return (res_builder::GenerateErrorResponse(clt));
		}
		ParseResponseTmp(response_tmp); // modify the clt->response by the received message from cgi
		std::string res_content_type = response_tmp.substr(response_tmp.find("Content-Type: "),response_tmp.find("\r\n\r\n")); // TODO: get the content type from the string returned by cgi
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

void	process::ProcessPostRequestCgi(struct Client *clt)
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
		std::vector<char*> cgi_argv = ConstructExecArray(clt->cgi_argv);
		SetCgiEnv(clt);
		std::vector<char*> cgi_env = ConstructExecArray(clt->cgi_env);
		if (cgi_argv.empty() || cgi_env.empty())
		{
			close(cgi_input[PipeEnd::kRead]);
			close(cgi_output[PipeEnd::kWrite]);
			exit(1);
		}
		execve(cgi_argv.data()[0], cgi_argv.data(), cgi_env.data());
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
	int write_byte = write(cgi_input[PipeEnd::kWrite], clt->req->getRequestBody().c_str(), clt->req->getRequestBody().size());
	if (write_byte < 0)
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
		int read = ReadAll(cgi_output[PipeEnd::kRead], response_tmp);
		close(cgi_output[PipeEnd::kRead]);
		if (read < 0)
		{
			clt->status_code = k500;
			return (res_builder::GenerateErrorResponse(clt));
		}
		ParseResponseTmp(); // modify the clt->response by the received message from cgi
		std::string res_content_type = response_tmp.substr(response_tmp.find("Content-Type: "),response_tmp.find("\r\n\r\n")); // TODO: get the content type from the string returned by cgi
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
int process::SetPipes(int *cgi_input, int *cgi_output, const Method method)
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

std::vector<char *>	process::ConstructExecArray(std::vector<std::string> &cgi_params) //{extension, cgi_path, NULL}
{
	std::vector<char*> cstrings;
	int cstrs_size = StringVecToTwoDimArray(cstrings, cgi_params);
	if (cstrs_size == 1)
	{
		std::cerr << "Error: ConstructCgiArgv" << std::endl;
		return (std::vector<char*>());
	}
	return cstrings;
}

void	process::SetCgiEnv(struct Client *clt)
{
	//env: method, query_string, content_length, content_type,
	//request_uri, document_uri, document_root, script_name,
	//server_addr, server_port, server_protocol, server_name

}
// fastcgi_param  SCRIPT_FILENAME    $document_root$fastcgi_script_name;
// fastcgi_param  QUERY_STRING       $query_string;
// fastcgi_param  REQUEST_METHOD     $request_method;
// fastcgi_param  CONTENT_TYPE       $content_type;
// fastcgi_param  CONTENT_LENGTH     $content_length;
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

//helper functions
int	process::StringVecToTwoDimArray(std::vector<char *> &cstrings,const std::vector<std::string> &strings)
{
	size_t vector_size = strings.size();
	std::cout << vector_size << std::endl;
	cstrings.reserve(vector_size + 1);
	for(size_t i = 0; i < vector_size; ++i)
		cstrings.push_back(const_cast<char*>(strings[i].c_str()));
	cstrings.push_back(nullptr);
	return cstrings.size();
}
