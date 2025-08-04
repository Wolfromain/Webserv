#include "../include/Server.hpp"

std::vector<Server*> g_servers;

int main()
{
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGPIPE, signalHandler);

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
	for (size_t i = 0; i < servers.size(); ++i)
	{
		Server* server = new Server(servers[i]);
		g_servers.push_back(server);
		server->start();
	}
	return (0);
}