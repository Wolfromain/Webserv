#include "utils.hpp"

std::string	readFile(const std::string &path)
{
	std::ifstream file(path.c_str());
	if (!file.is_open())
		return "";
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}

void	signalHandler(int signum)
{
	if (signum == SIGINT || signum == SIGTERM)
	{
		// a gerer
	}
}