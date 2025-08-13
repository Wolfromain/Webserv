#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <vector>
# include <cstring>
# include <unistd.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <signal.h>
# include <poll.h>

# include "Request.hpp"
# include "Config.hpp"
# include "utils.hpp"
# include "CGI.hpp"

class Request;

struct Location
{
	std::string path;
	std::string root;
	std::string index;
	std::string cgi_extension;
	std::vector<std::string> allow_methods;
	bool autoindex;
};

class Server
{
	private:

	int _server_fd;
	struct sockaddr_in _address;

	public:

		Server();
		Server(const Server& conf);
		~Server();
		int listen_port;
		std::string server_name;
		std::string root;
		std::map<int, std::string> error_pages;
		std::vector<Location> locations;
		size_t max_body_size;

		void start();
		void stop();
		bool isRunning() const;
		int	getServerFd() const;
};

#endif