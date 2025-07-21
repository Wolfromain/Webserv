#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>

# include "Server.hpp"

class Server;

class Config
{
	private:

		std::vector<Server> _servers;

	public:

		Config();
		~Config();

		int parseConfigFile(const std::string &filename);
		int parseServerBlock(std::ifstream &file);
		void parseLocationBlock(std::ifstream &file, Server &server, const std::string &firstLine);
		const std::vector<Server>& getServers() const;
		void cleanValue(std::string &value);
};

#endif