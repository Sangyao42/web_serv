#include "Client.hpp"

#include <cassert>

void	process::ProcessGetRequestCgi(struct Client *clt);
{
	int	cgi_input[2], cgi_output[2];
	int pipes = SetPipes(cgi_input[2], cgi_output[2], clt->req->getMethods()); //For CGI GET request, only cgi_output[2] is needed
	if (pipes < 0)
	{
		clt->statusCode = k500;
		return (generateErrorResponse(clt));
	}
	int pid = fork();
	if (pid < 0)
	{
		close(cgi_output[PipeEnd::kRead]);
		close(cgi_output[PipeEnd::kWrite]);
		clt->statusCode = k500;
		return (generateErrorResponse(clt));
	}
	if (pid == 0)
	{
		//child process
		close(cgi_output[PipeEnd::kRead]);
		dup2(cgi_output[PipeEnd::kWrite], STDOUT);
		CheckCgiFileExtension(); // get which cgi it is
		char* cgi_path = ConstructCgiExcutable(); // get excutable from location->cgis based on the file extension and check permission
		if (!executable_file_is_X_OK || !cgi_is_R_OK)
		{
			close(cgi_output[WRITE]);
			clt->statusCode = k500;
			return (generateErrorResponse(clt));
		}
		char** cgi_argv = ConstructCgiArgv();
		char** cgi_env = ConstructCgiEnv();
		execve(cgi_path, cgi_argv, cgi_env);
		// TODO: if execve failed, clean cgi_argv and cgi_env, and close the pipe
		close(cgi_output[WRITE]);
	}
	//parent process
	std::string response_tmp;
	close(cgi_output[WRITE]);
	int wstats;
	waitpid(pid, &wstats, 0);
	if (WIFEXITED(wtsats) && WEXITSTATUS(wstats) == 0)
	{
		int read = ReadAll(cgi_output[READ], response_tmp);
		close(cgi_output[READ]);
		if (read < 0)
		{
			clt->statusCode = k500;
			return (generateErrorResponse(clt));
		}
		ParseResponseTmp(); // modify the clt->response by the received message from cgi
		//generate responses based on the status code read from the cgi output
		if (clt->statusCode && clt->statusCode >= k400)
			return (generateErrorResponse(clt));
		if (content-type != clt->req->returnValueAsPointer("Accept"))
		{
			clt->statusCode = k406;
			return (generateErrorResponse(clt));
		}
		else
		{
			clt->statusCode = k200;
			return (generateSuccessResponse(clt))
		}
	}
	else
	{
		clt->statusCode = k500;
		return (generateErrorResponse(clt));
	}
}

int SetPipes(int *cgi_input, int *cgi_output, const std::string &method)
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