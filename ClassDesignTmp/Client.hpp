#pragma once

#include <iterator>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Request.hpp"
#include "Response.hpp"
#include "Protocol.hpp"
#include "SocketManager.hpp"
#include "Configuration.hpp"

enum ResponseError
{
	kNoError,
	kFileOpenError,
	kFilestreamError
};

struct Client
{
	status_code	status_code;
	struct ClientSocket	*client_socket;
	const struct ConfigurationQueryResult	*config;
	struct stat	stat_buff;
	std::string path;
	std::string cgi_executable; //path to cgi executable
	bool	keepAlive = true;
	Request	*req;
	Response	*res;
};

namespace process
{
	void	ProcessRequest(struct Client *clt);
	void	ProcessGetRequest(struct Client *clt);
	void	ProcessPostRequest(struct Client *clt);
	void	ProcessDeleteRequest(struct Client *clt);

	void	ProcessGetRequestCgi(struct Client *clt);
	void	ProcessPostRequestCgi(struct Client *clt);

	//file and path related functions
	std::string GetExactPath(const std::string root, std::string match_path, const struct Uri uri);
	bool		IsCgi(std::string &cgi_executable, std::string path, cache::LocationQuery *location);
	std::string	GetResContentType(std::string path);
	bool		IsAccessable(std::string content_type, HeaderValue *accept, cache::LocationQuery *location);
	std::string	GetIndexPath();
	bool		IsSupportedMediaType(std::string res);
	bool		IsDirFormat(std::string path);


	//cgi related functions
	enum PipeEnd
	{
		kRead,
		kWrite
	};
	int		SetPipes(int *cgi_input, int *cgi_output, const Method method);
	char*	ConstructCgiExcutable();
	char*	ConstructCgiPath();
	char**	ConstructCgiArgv(char* cgi_path);
	char**	ConstructCgiEnv();
	int 	ReadAll(int fd, std::string &response_tmp);

	//helper functions
	int		FreeTwoDimArray(char **argv);
}

namespace res_builder
{
	void	GenerateErrorResponse(struct Client *clt);
	void	GenerateRedirectResponse(struct Client *clt);
	void	GenerateChunkedResponse(struct Client *clt);
	void	GenerateAutoindexResponse(struct Client *clt);
	void	GenerateSuccessResponse(struct Client *clt);

	namespace helper
	{
		void	BuildErrorHeaders(struct Client *clt);
		enum ResponseBuilder	BuildErrorPage(const struct Client *clt);
	}

	namespace utils
	{
		void	BuildStatusLine(const struct Client *clt, std::string &response);
		const std::string &ReadFileToString(const std::string &path);
		const std::string	&StatusCodeAsString(enum status_code code);
	}

}
