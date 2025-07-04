#include "../include/Config.hpp"
#include "../include/Server.hpp"

Config::Config()
{

}

Config::~Config()
{

}

int Config::parseConfigFile(const std::string &filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Error: Could not open config file: " << filename << std::endl;
		return -1;
	}
	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
			continue;
		if (line.find("server") != std::string::npos)
			parseServerBlock(file);
	}
	return (0);
}

int Config::parseServerBlock(std::ifstream &file)
{
	Server server;
	std::string line;
	while (std::getline(file, line))
	{
		if (line.find("}") != std::string::npos)
			break;
		if (line.empty() || line[0] == '#')
			continue;

		
		if (line.find("listen") != std::string::npos)
			server.listen_port = atoi(line.substr(line.find("listen") + 6).c_str());
		if (line.find("server_name") != std::string::npos)
			server.server_name = line.substr(line.find("server_name") + 12);
	}
	_servers.push_back(server);
	return 0;
}