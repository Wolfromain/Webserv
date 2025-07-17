#include "Server.hpp"
#include "Reponse.hpp"

Server::Server()
{
}

Server::Server(const Server& cpy)
{
	*this = cpy;
	_server_fd = -1;
}

Server::~Server()
{
}

void Server::start()
{
	bool is_running = true;
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
	_address.sin_port = htons(listen_port);
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

	std::cout << "Server started on port " << listen_port << std::endl;

	std::vector<struct pollfd> fds;
	struct pollfd server_pollfd = {_server_fd, POLLIN, 0};
	fds.push_back(server_pollfd);

	while (is_running)
	{
		int poll_count = poll(fds.data(), fds.size(), 0);
		if (poll_count == -1)
		{
			std::cerr << "Error : poll()" << std::endl;
			break;
		}
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == _server_fd)
				{
					socklen_t addrlen = sizeof(_address);
					int client_fd = accept(_server_fd, (struct sockaddr*)&_address, &addrlen);
					if (client_fd == -1)
					{
						std::cerr << "Failed to accept connection" << std::endl;
						break;
					}
					struct pollfd client_pollfd = {client_fd, POLLIN, 100};
					fds.push_back(client_pollfd);
					std::cout << "New client connected: FD " << client_fd << std::endl;
				}
				else
				{
					char buffer[30720] = {0};
					is_read = read(fds[i].fd, buffer, sizeof(buffer));
					if (is_read < 0)
					{
						std::cerr << "Failed to read from socket" << std::endl;
						close(fds[i].fd);
						fds.erase(fds.begin() + i);
						--i;
						break;
					}
					if (is_read > 0 && is_read < (int)sizeof(buffer))
						buffer[is_read] = '\0';

					std::cout << "Received: " << buffer << std::endl;
					Request	request;
					std::cout << "AVANT TEST" << std::endl;
					request.parse(buffer);
					request.printAllToTest();

					Reponse rep;
					std::string reponse = rep.handleRequest(request, *this);
					send(fds[i].fd, reponse.c_str(), reponse.size(), 0);
					close(fds[i].fd);
					fds.erase(fds.begin() + i);
					--i;
				}
			}
		}
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
		std::cout << "Server not running" << std::endl;
}
