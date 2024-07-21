#pragma once

#include <set>
#include <iterator>
#include <fstream>
#include <sstream>
#include <ctime>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include "Request.hpp"
#include "Response.hpp"
#include "Protocol.hpp"
#include "socket_manager/SocketManager.hpp"
#include "Configuration.hpp"
#include "constants.hpp"

typedef std::string DName; // for dirent/autoindex
typedef	unsigned char DType;
typedef std::pair<DType, DName> DPair;
typedef std::set<DPair> DSet;
typedef DSet::iterator DSetIt;

enum ResponseError
{
	kResponseNoError,
	kFileOpenError,
	kFileStreamError
};

struct Client
{
	StatusCode	status_code;
	struct ClientSocket	*client_socket;
	struct ConfigurationQueryResult	config;
	struct stat	stat_buff;
	std::string path;
	std::vector<std::string> cgi_argv; //path to cgi executable and path to cgi script
	std::vector<std::string> cgi_env;
	bool	keepAlive;
	std::string location_created;
	std::string cgi_content_type;
	int			cgi_content_length;
	//START: request status before processing
	size_t content_length;
	size_t max_body_size;
	bool continue_reading;
	bool exceed_max_body_size;
	bool is_chunked;
	bool is_chunk_end;
	bool consume_body;
	//END: request status before processing
	Request	req;
	Response	res;
};

namespace client_lifespan
{
	void	InitClient(struct Client &client, struct ClientSocket *client_socket);
  int   DeleteClientFromVector(std::vector<struct Client> &clients, int client_fd);
	void	ResetClient(struct Client &client);

	//update functions
	// void	UpdateStatusCode(struct Client &clt, StatusCode statuscode);
	void	CheckHeaderBeforeProcess(struct Client *clt);
	bool	IsClientAlive(struct Client *clt);

	//helper functions
	struct Client	*GetClientByFd(std::vector<struct Client> &clients, int client_fd);
}

namespace process
{
	void	ProcessRequest(struct Client *clt);
	void	ProcessGetRequest(struct Client *clt);
	void	ProcessPostRequest(struct Client *clt);
	void	ProcessDeleteRequest(struct Client *clt);

	//file and path and content-type related functions
	std::string GetExactPath(const std::string root, std::string match_path, const struct Uri uri);
	bool		IsCgi(std::vector<std::string> &cgi_executable, std::string path, cache::LocationQuery *location);
	std::string	GetReqExtension(std::string path);
	// bool		IsAcceptable(std::string content_type, HeaderValue *accept, cache::LocationQuery *location);
	std::string	GetIndexPath(std::string path, cache::LocationQuery *location);
	bool		IsSupportedMediaType(std::string req_content_type, const directive::MimeTypes* mime_types);
	bool		IsDirFormat(std::string path);

	namespace file
	{
		enum FileManiError
		{
			kFileManiNoError,
			kFileManiPermissionError,
			kFileManiOtherError
		};
		bool	ModifyFile(struct Client *clt);
		enum FileManiError	UploadFile(struct Client *clt);
		bool	DeleteFile(struct Client *clt);

		//helper functions
		std::string	GenerateFileName(std::string path); //base on timestamp
		std::string GenerateFileExtension(std::string content_type, const directive::MimeTypes* mime_types);
		enum FileManiError CreateDir(std::string dir);
		enum FileManiError CreateDirRecurs(std::string path);
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
	struct CgiOutput
	{
		std::string content_type;
		std::string content_location;
		std::string	content_body;
	};
	int		SetPipes(int *cgi_input, int *cgi_output, const Method method);
	void	SetCgiEnv(struct Client *clt);
	int		ReadAll(int fd, std::string &response_tmp);
	int		WriteAll(int fd, char *cstr_buf, int size);
	bool	ParseCgiOutput(struct CgiOutput &cgi_output, std::string &response_tmp);

	//helper functions
	char**	StringVecToTwoDimArray(std::vector<char *> &cstrings, const std::vector<std::string> &strings);
}

namespace res_builder
{
	void	GenerateErrorResponse(struct Client *clt);
	void	GenerateRedirectResponse(struct Client *clt);
	void	GenerateAutoindexResponse(struct Client *clt);
	void	GenerateSuccessResponse(struct Client *clt);

	// error page related helper functions
	void	BuildErrorHeaders(struct Client *clt);
	std::string BuildErrorPage(StatusCode code);

	// redirect related helper functions
	void	BuildRedirectResponseBody(struct Client *clt);

	// autoindex related helper functions
	std::string BuildAutoindexHTML(DSet files, std::string path);

	// success related helper functions
	void	BuildPostResponseBody(struct Client *clt);

	// header related helper functions
	void	AddLocationHeader(struct Client *clt);
	void	AddAllowHeader(struct Client *clt);
	void	AddAcceptHeader(struct Client *clt);
	void	BuildContentHeadersCGI(struct Client *clt);
	void	BuildContentHeaders(struct Client *clt, std::string extension, std::string path);

	// general utility functions
	std::string MethodToString(enum directive::Method method);
	void	ServerError500(struct Client *clt);
	std::string	GetTimeGMT();
	std::string GetTimeGMT(time_t raw_time);
	void	BuildBasicHeaders(Response *res);
	void	BuildStatusLine(StatusCode status_code, std::string &response);
	enum ResponseError	ReadFileToBody(const std::string &path, Response *res);
	std::string	StatusCodeAsString(StatusCode code);
}
