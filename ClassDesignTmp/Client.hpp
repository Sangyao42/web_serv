#pragma once

#include <iterator>
#include <fstream>
#include <sstream>
#include <ctime>
#include <sys/time.h>
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
	std::vector<std::string> cgi_argv; //path to cgi executable and path to cgi script
	std::vector<std::string> cgi_env;
	bool	keepAlive = true;
	std::string location_created;
	Request	*req;
	Response	*res;
};

namespace process
{
	void	ProcessRequest(struct Client *clt);
	void	ProcessGetRequest(struct Client *clt);
	void	ProcessPostRequest(struct Client *clt);
	void	ProcessDeleteRequest(struct Client *clt);

	//file and path and content-type related functions
	std::string GetExactPath(const std::string root, std::string match_path, const struct Uri uri);
	bool		IsCgi(std::vector<std::string> &cgi_executable, std::string path, cache::LocationQuery *location);
	std::string	GetResContentType(std::string path);
	bool		IsAcceptable(std::string content_type, HeaderValue *accept, cache::LocationQuery *location);
	std::string	GetIndexPath(std::string path, cache::LocationQuery *location);
	bool		IsSupportedMediaType(std::string req_content_type, const directive::MimeTypes* mime_types);
	bool		IsDirFormat(std::string path);

	namespace file
	{
		bool	ModifyFile(struct Client *clt);
		bool	UploadFile(struct Client *clt);
		bool	DeleteFile(struct Client *clt);

		//helper functions
		std::string	GenerateFileName(std::string path); //base on timestamp
		std::string GenerateFileExtension(std::string content_type, const directive::MimeTypes* mime_types);
		bool CreateDir(std::string dir);
		bool CreateDirRecurs(std::string path);
	}
}

namespace cgi
{
	void	ProcessGetRequestCgi(struct Client *clt);
	void	ProcessPostRequestCgi(struct Client *clt);

	enum PipeEnd
	{
		kRead,
		kWrite
	};
	int		SetPipes(int *cgi_input, int *cgi_output, const Method method);
	// std::vector<char *>	ConstructExecArray(std::vector<std::string> &cgi_params);
	void	SetCgiEnv(struct Client *clt);
	int		ReadAll(int fd, std::string &response_tmp);
	int 	WriteAll(int fd, char *cstr_buf, int size);

	//helper functions
	char**	StringVecToTwoDimArray(const std::vector<std::string> &strings);
}

namespace res_builder
{
	void	GenerateErrorResponse(struct Client *clt);
	void	GenerateRedirectResponse(struct Client *clt);
	void	GenerateAutoindexResponse(struct Client *clt);
	void	GenerateSuccessResponse(struct Client *clt);

	// error page related helper functions
	void	BuildErrorHeaders(struct Client *clt);
	const std::string &BuildErrorPage(enum status_code code);

	// redirect related helper functions

	// autoindex related helper functions

	// success related helper functions

	// header related helper functions
	void	AddAllowHeader(struct Client *clt);
	void	AddAcceptHeader(struct Client *clt);
	void	BuildContentHeadersCGI(struct Client *clt);
	void	BuildContentHeaders(struct Client *clt, std::string path);

	// general utility functions
	std::string MethodToString(enum directive::Method method);
	void	ServerError500(struct Client *clt);
	std::string	GetTimeGMT();
	std::string GetTimeGMT(time_t raw_time);
	void	BuildBasicHeaders(Response *res);
	void	BuildStatusLine(enum status_code status_code, std::string &response);
	enum ResponseError	ReadFileToBody(const std::string &path, Response *res);
	const std::string	&StatusCodeAsString(enum status_code code);
}
