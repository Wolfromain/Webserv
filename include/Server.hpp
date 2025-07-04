#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
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

		Server();
		~Server();

		void start();
		void stop();
		bool isRunning() const;
		int	getServerFd() const;
};

#endif