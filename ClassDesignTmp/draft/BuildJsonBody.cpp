std::string	res_builder::BuildJsonBodyPost(std::string path_resource)
{
	// k201
	{
			"id": 123,
			"message": "Resource created successfully.",
			"name": "New Resource",
			"updated_at": "2022-05-25T10:15:00Z",
			"links": {
					"self": "/path/to/resource"
			}
	}

	// k200
	{
		"id": 345,
		"message": "Resource modified successfully.",
		"name": "Modified Resource",
		"updated_at": "2022-05-25T10:15:00Z",
		"links": {
			"self": "path/to/resource"
		}
	}

	FileMetadata::FileId	file_id = FileMetadata::getFileId(path_resource);
	if (file_id == -1)
	{
		file_id = IDGenerator::generateID();
		FileMetadata::addFileId(path_resource, file_id);
	}

	std::string response = "{\n";
	response += "\"id\": " + std::string(file_id) + ",\n";
	response += "\"message\": \"Resource created successfully.\",\n";
	response += "\"name\": \"New Resource\",\n";
	response += "\"updated_at\": \"" + GetTimeGMT() + "\",\n";
	response += "\"links\": {\n";
	response += "\"self\": \"" + path_resource + "\"\n";
	response += "}\n";
	response += "}\n";

	return (response);
}