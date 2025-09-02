#include "../include/Server.hpp"
#include "../include/utils.hpp"

std::vector<Server*> g_servers;
bool g_running = true;

void signalHandler(int signum)
{
	if (signum == SIGINT || signum == SIGTERM)
	{
		g_running = false;
	}
	else if (signum == SIGPIPE)
		return;
}

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

	while (g_running)
	{
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
	}

	std::cout << "\nShutting down server..." << std::endl;
	for (std::vector<Server*>::iterator it = g_servers.begin(); it != g_servers.end(); ++it)
	{
		if (*it)
		{
			(*it)->stop();
			delete *it;
		}
	}
	g_servers.clear();
	std::cout << "Server shutdown complete." << std::endl;

	return 0;
}