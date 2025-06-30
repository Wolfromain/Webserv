#include <Server.hpp>

#define PORT 8080

Server::Server()
{
	_port = PORT;
}

Server::~Server()
{
	stop();
}

void Server::start()
{
	int is_Result = 0;
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd == 0)
		//ERROR
	_address.sin_family = AF_INET;
	_address.sin_port = htons(8080);
	_address.sin_addr.s_addr = INADDR_ANY;

	bind(_server_fd, (struct sockaddr*)&_address, sizeof(_address));
	listen(_server_fd, SOMAXCONN);
}

void Server::stop()
{
}