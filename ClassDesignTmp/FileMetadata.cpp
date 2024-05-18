#include "FileMetadata.hpp"

void FileMetadata::addFileId(FileMetadata::FilePath path, FileMetadata::FileId id)
{
	file_id_map[path] = id;
}

void FileMetadata::removeFileId(FileMetadata::FilePath path)
{
	file_id_map.erase(path);
}

void FileMetadata::updateFilePath(FileMetadata::FilePath old_path, FileMetadata::FilePath new_path)
{
	FileMetadata::FileIdMapIt	it;

	it = file_id_map.find(old_path);
	if (it != file_id_map.end())
	{
		file_id_map[new_path] = it->second;
		file_id_map.erase(it);
	}
}

FileMetadata::FileId FileMetadata::getFileId(FileMetadata::FilePath path)
{
	FileMetadata::FileIdMapIt	it;

	it = file_id_map.find(path);
	if (it != file_id_map.end())
		return (it->second);
	else
		return (-1);
}
