#include "../include/Server.hpp"

int main()
{

	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	Config config;
	if (config.parseConfigFile("config/webserv.conf") != 0)
	{
		std::cerr << "Erreur lors du parsing du fichier de configuration." << std::endl;
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