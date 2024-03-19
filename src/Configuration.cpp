#include <Configuration.hpp>
#include <vector>
#include <iostream>

//getters
int Configuration::getMaxClients() const
{
	return max_clients;
}

std::vector<int> Configuration::getPorts() const
{
	return ports;
}