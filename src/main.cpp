#include "../include/Server.hpp"

int main()
{

	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	Config config;
	if (!std::ifstream("config/webserv.conf"))
	{
		std::cerr << "The file config/webserv.conf does not exist." << std::endl;
		return (1);
	}

	if (config.parseConfigFile("config/webserv.conf") != 0)
	{
		std::cerr << "Error : parsing .conf." << std::endl;
		return (1);
	}

	const std::vector<Server>& servers = config.getServers();
	std::vector<Server> runningServers;
	for (size_t i = 0; i < servers.size(); ++i) //Si on fait plusieurs serveurs
	{
		runningServers.push_back(Server(servers[i]));
		runningServers.back().start();
	}

	return (0);
} 