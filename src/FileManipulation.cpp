#include "Client.hpp"

#include <string.h>
#include <cassert>

bool process::file::ModifyFile(struct Client *clt)
{
	std::ofstream file(clt->path.c_str());
	if (!file.is_open())
	{
		return false;
	}
	file << clt->req.getRequestBody();
	if (file.good())
	{
		file.close();
		return true;
	}
	file.close();
	return false;
}

bool process::file::UploadFile(struct Client *clt)
{
	//example path1: ../www/html/files/
	//example path2: ../www/html/files/filename.txt
	//example path3: ../www/html/files/layer/filename

	//if path is a directory, generate a file name without extension based on timestamp
	std::string file_path = clt->path;
	if (IsDirFormat(clt->path))
		file_path += GenerateFileName(file_path);

	//if no file extension, add one based on content-type
	size_t pos = file_path.find_last_of(".");
	if (pos != std::string::npos && file_path[pos + 1] == '/')
	{
		HeaderString *content_type = static_cast<HeaderString *>(clt->req.returnValueAsPointer("Content-Type"));
		if (content_type)
		{
			file_path += ".";
			file_path += GenerateFileExtension(content_type->content(), clt->config.query->mime_types);
		}
	}

	//create file and all the directories in the path
	if (!CreateDirRecurs(file_path))
		return false;
	std::ofstream file(file_path.c_str());
	if (!file.is_open())
		return false;
	//write to file
	file << clt->req.getRequestBody();
	if (!file.good())
	{
		file.close();
		return false;
	}
	file.close();

	//Write to the location_created variable in the client struct
	//the full path for gettting info about the file later when generating response
	// size_t root_pos = file_path.find(clt->config->query->root) + clt->config->query->root.size() - 1;
	// clt->location_created = file_path.substr(root_pos);
	clt->location_created = file_path;
	return true;
}

bool process::file::DeleteFile(struct Client *clt)
{
	if (remove(clt->path.c_str()) != 0)
		return false;
	return true;
}

//helper functions
std::string process::file::GenerateFileName(std::string path)
{
	(void) path;
	std::time_t	time = std::time(NULL);
	char time_buf[80];
	std::strftime(time_buf, 80, "%Y-%m-%dT%H-%M-%SZ", std::gmtime(&time));
	return (std::string(time_buf));
}

std::string	process::file::GenerateFileExtension(std::string content_type, const directive::MimeTypes* mime_types)
{
	const std::map<std::string, std::string> &mime_map = mime_types->get();
	std::map<std::string, std::string>::const_iterator it;
	for (it = mime_map.begin(); it != mime_map.end(); it++)
	{
		if (it->second == content_type)
		{
			return it->first;
		}
	}
	return "";
}

bool process::file::CreateDir(std::string dir)
{
	struct stat buffer;
	int status = stat(dir.c_str(), &buffer);
	if (status == 0 && S_ISDIR(buffer.st_mode))
	{
		std::cout << "Directory exists" << std::endl;
		return true;
	}
	else
	{
		// std::cout << path << std::endl;
		if (mkdir(dir.c_str(), 0777) != 0)
		{
			std::cout << "Error creating directory" << std::endl;
			std::cerr << strerror(errno) << std::endl;
			return false;
		}
		else
		{
			std::cout << "Directory created" << std::endl;
			return true;
		}
	}
}

bool process::file::CreateDirRecurs(std::string path)
{
	size_t pos = 0;
	pos = path.find_first_of("/", pos + 1);
	std::string create_dir = path.substr(0, pos);
	std::cout << create_dir << std::endl;
	while (pos != std::string::npos)
	{
		if (!CreateDir(create_dir))
		{
			return false;
		}
		pos = path.find_first_of("/", pos + 1);
		create_dir = path.substr(0, pos);
		std::cout << create_dir << std::endl;
	}
	return true;
}
