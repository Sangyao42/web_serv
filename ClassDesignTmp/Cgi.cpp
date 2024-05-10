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
		return (resBuilder::GenerateErrorResponse(clt));
	}
	int pid = fork();
	if (pid < 0)
	{
		close(cgi_output[PipeEnd::kRead]);
		close(cgi_output[PipeEnd::kWrite]);
		clt->status_code = k500;
		return (resBuilder::GenerateErrorResponse(clt));
	}
	if (pid == 0)
	{
		//child process
		close(cgi_output[PipeEnd::kRead]);
		dup2(cgi_output[PipeEnd::kWrite], STDOUT_FILENO);
		char* cgi_executable = ConstructCgiExcutable(); // get excutable from looping through location->cgis based on the file extension and check permission
		char* cgi_path = ConstructCgiPath(); // get excutable from looping through location->cgis based on the file extension and check permission
		if (access(cgi_executable, X_OK) != 0 || access(cgi_path, R_OK) != 0)
		{
			close(cgi_output[PipeEnd::kWrite]);
			exit(1);
		}
		char** cgi_argv = ConstructCgiArgv(cgi_path);
		char** cgi_env = ConstructCgiEnv();
		if (cgi_argv == NULL || cgi_env == NULL)
		{
			close(cgi_output[PipeEnd::kWrite]);
			exit(1);
		}
		execve(cgi_executable, cgi_argv, cgi_env);
		int checkfree = FreeTwoDimArray(cgi_argv);
		int checkfree2 = FreeTwoDimArray(cgi_env);
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
			return (resBuilder::GenerateErrorResponse(clt));
		}
		ParseResponseTmp(response_tmp); // modify the clt->response by the received message from cgi
		if (IsAccessable("content-type")) // TODO: get the content type from the string returned by cgi
		{
			clt->status_code = k406;
			return (resBuilder::GenerateErrorResponse(clt));
		}
		else
		{
			clt->status_code = k200;
			return (resBuilder::GenerateSuccessResponse(clt));
		}
	}
	else
	{
		clt->status_code = k500;
		return (resBuilder::GenerateErrorResponse(clt));
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

char*	process::ConstructCgiPath()
{
}

char*	process::ConstructCgiExcutable()
{
	// get excutable from looping through location->cgis based on the file extension and check permission
}

char**	process::ConstructCgiArgv(char* cgi_path)
{
}

char**	process::ConstructCgiEnv()
{
}

//helper functions
int	process::FreeTwoDimArray(char **argv)
{
}