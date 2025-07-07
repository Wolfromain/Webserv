#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <vector>
# include <cstring>
# include <unistd.h>
# include <netinet/in.h>
# include <sys/socket.h>

# include "Request.hpp"
# include "Reponse.hpp"

# define PORT 8080

class Request;

class Server
{
	private:

	int _server_fd;
	int _port;
	struct sockaddr_in _address;

	public:

		struct Location
		{
			std::string path;
			std::string root;
			std::string index;
			std::vector<std::string> allow_methods;
			bool autoindex;
		};

		Server();
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