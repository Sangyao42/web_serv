#include "Client.hpp"
#include <cassert>

void	res_builder::GenerateAutoindexResponse(struct Client *clt)
{
	DIR	*dir_stream = opendir(clt->path.c_str());

	if (dir_stream == NULL)
	{
		assert(errno == ENOENT || errno == ENOTDIR || errno == EACCES && "Unexpected error opening directory");
		ServerError500(clt);
		return ;
	}
}