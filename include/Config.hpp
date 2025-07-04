#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

# include "Server.hpp"

class Config
{
	private:

		std::vector<Server> _servers;

	public:

		Config();
		~Config();

		int parseConfigFile(const std::string &filename);
		int parseServerBlock(std::ifstream &file);
		std::vector<Server> getServers() const;
};

#endif