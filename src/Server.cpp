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
					char buffer[30720];
					std::string request_data;
					int is_read = 0;
					// 1. Lire tout ce qui est dispo (headers + body partiel)
					is_read = read(fds[i].fd, buffer, sizeof(buffer));
					if (is_read < 0)
					{
						std::cerr << "Failed to read from socket" << std::endl;
						close(fds[i].fd);
						fds.erase(fds.begin() + i);
						--i;
						break;
					}
					request_data.append(buffer, is_read);

					// 2. Trouver la fin des headers
					size_t header_end = request_data.find("\r\n\r\n");
					if (header_end == std::string::npos)
					{
						continue;
					}

					// 3. Récupérer Content-Length
					size_t content_length = 0;
					size_t pos = request_data.find("Content-Length:");
					if (pos != std::string::npos)
					{
						size_t end = request_data.find("\r\n", pos);
						std::string len_str = request_data.substr(pos + 15, end - (pos + 15));
						content_length = atoi(len_str.c_str());
					}

					// 4. Calculer combien d'octets il manque pour le body
					size_t body_start = header_end + 4;
					size_t body_received = request_data.size() - body_start;
					while (body_received < content_length)
					{
						is_read = read(fds[i].fd, buffer, sizeof(buffer));
						if (is_read <= 0) break;
						request_data.append(buffer, is_read);
						body_received = request_data.size() - body_start;
					}

					// 5. Passer la requête complète à Request::parse
					Request request;
					request.parse(request_data);
					request.printAllToTest();

					Reponse rep;
					std::string reponse = rep.handleRequest(request, *this);
					std::string http_status = "HTTP/1.1 200 OK\r\n";
					std::string full_response = http_status + reponse;
					send(fds[i].fd, full_response.c_str(), full_response.size(), 0);
					close(fds[i].fd);
					fds.erase(fds.begin() + i);
					--i;
				}
			}
		}
	}
	fds.clear();
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
