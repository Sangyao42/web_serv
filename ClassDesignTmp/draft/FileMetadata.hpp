#pragma once

#include <string>
#include <map>

namespace FileMetadata
{
	typedef std::string FilePath;
	typedef int	FileId;
	typedef std::map<FileMetadata::FilePath, FileMetadata::FileId>	FileIdMap;
	typedef std::map<FileMetadata::FilePath, FileMetadata::FileId>::iterator	FileIdMapIt;

	void addFileId(FileMetadata::FilePath path, FileMetadata::FileId id);
	void removeFileId(FileMetadata::FilePath path);
	void updateFilePath(FileMetadata::FilePath old_path, FileMetadata::FilePath new_path);
	FileMetadata::FileId getFileId(FileMetadata::FilePath path);
};

extern FileMetadata::FileIdMap file_id_map;
