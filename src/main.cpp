#include "../include/Server.hpp"

std::vector<Server*> g_servers;
int main(int argc, char **argv)
{
	const char* config_file = "config/webserv.conf";

	if (argc > 2)
	{
		std::cerr << "Usage: ./webserv [configuration file]" << std::endl;
		return (1);
	}
	else if (argc == 2)
	{
		config_file = argv[1];
	}

	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGPIPE, signalHandler);

	Config config;
	if (!std::ifstream(config_file))
	{
		std::cerr << "The file " << config_file << " does not exist." << std::endl;
		return (1);
	}

	if (config.parseConfigFile(config_file) != 0)
	{
		std::cerr << "Error : parsing .conf." << std::endl;
		return (1);
	}

	const std::vector<Server>& servers = config.getServers();
	for (size_t i = 0; i < servers.size(); ++i)
	{
		// std::cout << "Server " << i << " has " << servers[i].getPorts().size() << " _ports" << std::endl;
		Server* server = new Server(servers[i]);
		// std::cout << "After copy, server has " << server->getPorts().size() << " _ports" << std::endl;
		g_servers.push_back(server);
		server->initSocket();
	}
	Server::runPollLoop(g_servers);

	// for (size_t i = 0; i < servers.size(); ++i)
	// {
	// 	std::cout << "Server " << i << " has " << servers[i]._ports.size() << " _ports" << std::endl;
	// 	Server* server = new Server(servers[i]);
	// 	std::cout << "After copy, server has " << server->_ports.size() << " _ports" << std::endl;
	// 	g_servers.push_back(server);
	// 	server->start();
	// }

	return (0);
}