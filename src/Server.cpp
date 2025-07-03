#include "Server.hpp"

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
	bool is_running = false; //GLOBAL
	int is_read = 0;
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd == -1)
	{
		std::cerr << "Failed to create socket" << std::endl;
		return;
	}

	int opt = 1;
	setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	_address.sin_family = AF_INET;
	_address.sin_port = htons(PORT);
	_address.sin_addr.s_addr = INADDR_ANY;

	if (bind(_server_fd, (struct sockaddr*)&_address, sizeof(_address)) == -1)
	{
		std::cerr << "Failed to bind socket" << std::endl;
		return;
	}

	if (listen(_server_fd, SOMAXCONN) == -1)
	{
		std::cerr << "Failed to listen on socket" << std::endl;
		return;
	}
	is_running = true;
	std::cout << "Server started on port " << _port << std::endl;
	while (is_running)
	{
		socklen_t addrlen = sizeof(_address);
		int client_fd = accept(_server_fd, (struct sockaddr*)&_address, &addrlen);
		if (client_fd == -1)
		{
			std::cerr << "Failed to accept connection" << std::endl;
			break;
		}

		char buffer[30720] = {0};
		is_read = read(client_fd, buffer, sizeof(buffer));
		if (is_read < 0)
		{
			std::cerr << "Failed to read from socket" << std::endl;
			close(client_fd);
			break;
		}

		std::cout << "Received: " << buffer << std::endl;
		Request	request;
		request.parse(buffer);

		const char* response = "TEST";
		send(client_fd, response, strlen(response), 0);
		close(client_fd);
	}
	stop();
}

bool Server::isRunning() const
{
	if (_server_fd != -1)
		return true;
	return false;
}

int Server::getServerFd() const
{
	return (_server_fd);
}

void Server::stop()
{
	if (_server_fd != -1)
	{
		close(_server_fd);
		_server_fd = -1;
		std::cout << "Server closed" << std::endl;
	}
	else
	{
		std::cout << "Server not running" << std::endl;
	}
}