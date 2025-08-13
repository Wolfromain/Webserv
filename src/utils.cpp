#include "utils.hpp"

extern std::vector<Server*> g_servers;

std::string	readFile(const std::string &path)
{
	std::ifstream file(path.c_str());
	if (!file.is_open())
		return "";
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}

void signalHandler(int signum)
{
	if (signum == SIGINT || signum == SIGTERM)
	{
		std::cout << "\nShutting down server..." << std::endl;
		for (size_t i = 0; i < g_servers.size(); i++)
		{
			if (g_servers[i] && g_servers[i]->isRunning())
				g_servers[i]->stop();
		}
		for (size_t i = 0; i < g_servers.size(); i++)
			delete g_servers[i];
		g_servers.clear();

		std::cout << "Server shutdown complete." << std::endl;
		exit(0);
	}
	else if (signum == SIGPIPE)
		return;
}