#include "../include/Config.hpp"
#include "../include/Server.hpp"

Config::Config()
{

}

Config::~Config()
{

}

const std::vector<Server>& Config::getServers() const
{
	return (_servers);
}

int Config::parseConfigFile(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		std::cerr << "Error: Could not open config file: " << filename << std::endl;
		return (-1);
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
		else if (line.find("server_name") != std::string::npos)
		{
			std::string value = line.substr(line.find("server_name") + 11);
			cleanValue(value);
			server.server_name = value;
		}
		else if (line.find("root") != std::string::npos)
		{
			std::string value = line.substr(line.find("root") + 4);
			cleanValue(value);
			server.root = value;
		}
		else if (line.find("error_page") != std::string::npos)
		{
			std::istringstream iss(line);
			std::string code, path, token;
			iss >> token;
			while (iss >> code)
			{
				if (code[0] == '/')
					path = code;
				else
					server.error_pages[atoi(code.c_str())] = "";
			}
			if (!path.empty())
				for (std::map<int, std::string>::iterator it = server.error_pages.begin(); it != server.error_pages.end(); ++it)
					if (it->second.empty())
						it->second = path;
		}
		else if (line.find("max_body_size") != std::string::npos)
		{
			std::string value = line.substr(line.find("max_body_size") + 13);
			size_t res = 1;
			if (value.find("M") != std::string::npos)
				res = 1024 * 1024;
			else if (value.find("K") != std::string::npos)
				res = 1024;
			server.max_body_size = atoi(value.c_str()) * res;
		}
		else if (line.find("location") != std::string::npos)
			parseLocationBlock(file, server, line);
	}
	_servers.push_back(server);
	return (0);
}

void Config::parseLocationBlock(std::ifstream &file, Server &server, const std::string &firstLine)
{
	Location locations;
	std::istringstream iss(firstLine);
	std::string tmp;
	iss >> tmp >> locations.path;
		
	std::string line;
	while (std::getline(file, line))
	{
		if (line.find('}') != std::string::npos)
			break;
		if (line.empty() || line[0] == '#')
			continue;
			
		if (line.find("root") != std::string::npos)
		{
			std::string value = line.substr(line.find("root") + 4);
			cleanValue(value);
			locations.root = value;
		}
		else if (line.find("index") != std::string::npos)
		{
			std::string value = line.substr(line.find("index") + 5);
			cleanValue(value);
			locations.index = value;
		}
		else if (line.find("allow_methods") != std::string::npos)
		{
			std::istringstream iss2(line);
			std::string keyword, method;
			iss2 >> keyword;
			while (iss2 >> method)
			{
				if (!method.empty() && method[method.length()-1] == ';')
					method.erase(method.length()-1);
				locations.allow_methods.push_back(method);
			}
		}
		else if (line.find("autoindex") != std::string::npos)
			locations.autoindex = (line.find("on") != std::string::npos);
		else if (line.find("cgi_extension") != std::string::npos)
		{
			std::string value = line.substr(line.find("cgi_extension") + 13);
			cleanValue(value);
			locations.cgi_extension = value;
		}
		else if (line.find("return") != std::string::npos)
		{
			std::istringstream iss(line);
			std::string keyword;
			iss >> keyword;
			
			iss >> locations.redirectCode >> locations.redirectPath;
			
			// Nettoie le point-virgule
			if (!locations.redirectPath.empty() && locations.redirectPath[locations.redirectPath.length()-1] == ';')
				locations.redirectPath.erase(locations.redirectPath.length()-1);
				
			locations.hasRedirect = true;
		}
	}
	server.locations.push_back(locations);
}

void Config::cleanValue(std::string &value)
{
	while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
		value = value.substr(1);
		
	while (!value.empty() && (value[value.length()-1] == ' ' || 
			value[value.length()-1] == '\t' || 
			value[value.length()-1] == ';' ||
			value[value.length()-1] == '\r' ||
			value[value.length()-1] == '\n'))
		value.erase(value.length()-1);
}