#include <Configuration.hpp>

#include <vector>
#include <iostream>

Configuration::Configuration() : max_clients(10), serv_ports({8080, 8081, 8082, 8083, 8084})
{
}

//getters
int Configuration::getMaxClients() const
{
	return max_clients;
}

std::vector<int> Configuration::getPorts() const
{
	return serv_ports;
}