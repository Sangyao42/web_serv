#pragma once

#include <vector>

class Configuration
{
	public:
		//getters
		int getMaxClients() const;
		std::vector<int> getPorts() const;

	private:
		const int max_clients;
		std::vector<int> ports;
};