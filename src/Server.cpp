#include "Server.hpp"
#include "Reponse.hpp"

static std::map<int, Server*> client_to_server;

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

// void Server::initSocket()
// {
// 	for (size_t i = 0; i < _ports.size(); i++)
// 	{
// 		int sock = socket(AF_INET, SOCK_STREAM, 0);
// 		if (sock == -1)
// 		{
// 			std::cerr << "Failed to create socket for port " << _ports[i] << std::endl;
// 			continue;
// 		}

// 		int opt = 1;
// 		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

// 		struct sockaddr_in addr;
// 		std::memset(&addr, 0, sizeof(addr));
// 		addr.sin_family = AF_INET;
// 		addr.sin_port = htons(_ports[i]);
// 		addr.sin_addr.s_addr = INADDR_ANY;

// 		if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
// 		{
// 			std::cerr << "Failed to bind on port " << _ports[i] << std::endl;
// 			close(sock);
// 			continue;
// 		}

// 		if (listen(sock, SOMAXCONN) == -1)
// 		{
// 			std::cerr << "Failed to listen on port " << _ports[i] << std::endl;
// 			close(sock);
// 			continue;
// 		}

// 		std::cout << "Listening on port " << _ports[i] << std::endl;
// 		_listen_fds.push_back(sock);
// 	}
// }

// void Server::runPollLoop(std::vector<Server*>& servers)
// {
// 	std::vector<struct pollfd> fds;

// 	for (size_t s = 0; s < servers.size(); s++)
// 	{
// 		for (size_t i = 0; i < servers[s]->_listen_fds.size(); i++)
// 		{
// 			struct pollfd pfd = { servers[s]->_listen_fds[i], POLLIN, 0 };
// 			fds.push_back(pfd);
// 		}
// 	}

// 	bool is_running = true;
// 	while (is_running)
// 	{
// 		int poll_count = poll(fds.data(), fds.size(), -1);
// 		if (poll_count == -1)
// 		{
// 			std::cerr << "Error: poll()" << std::endl;
// 			break;
// 		}

// 		for (size_t i = 0; i < fds.size(); i++)
// 		{
// 			if (fds[i].revents & (POLLHUP | POLLERR))
// 			{
// 				close(fds[i].fd);
// 				client_to_server.erase(fds[i].fd);
// 				fds.erase(fds.begin() + i);
// 				--i;
// 				continue;
// 			}
// 			{
// 				bool is_server_fd = false;
// 				Server* current_server = NULL;

// 				for (size_t s = 0; s < servers.size(); s++)
// 				{
// 					if (std::find(servers[s]->_listen_fds.begin(),
// 								  servers[s]->_listen_fds.end(),
// 								  fds[i].fd) != servers[s]->_listen_fds.end())
// 					{
// 						is_server_fd = true;
// 						current_server = servers[s];
// 						break;
// 					}
// 				}

// 				if (is_server_fd)
// 				{
// 					struct sockaddr_in client_addr;
// 					socklen_t addrlen = sizeof(client_addr);
// 					int client_fd = accept(fds[i].fd, (struct sockaddr*)&client_addr, &addrlen);
// 					if (client_fd == -1)
// 					{
// 						std::cerr << "Failed to accept connection" << std::endl;
// 						continue;
// 					}

// 					client_to_server[client_fd] = current_server;

// 					struct pollfd client_pfd = { client_fd, POLLIN, 0 };
// 					fds.push_back(client_pfd);

// 					std::cout << "New client FD " << client_fd 
// 							  << " on server " << current_server->server_name 
// 							  << std::endl;
// 				}
// 				else
// 				{
// 					char buffer[30720];
// 					int is_read = read(fds[i].fd, buffer, sizeof(buffer));
// 					if (is_read <= 0)
// 					{
// 						close(fds[i].fd);
// 						client_to_server.erase(fds[i].fd);
// 						fds.erase(fds.begin() + i);
// 						--i;
// 						continue;
// 					}

// 					std::string request_data(buffer, is_read);

// 					Server* current_server = client_to_server[fds[i].fd];

// 					Request request;
// 					request.parse(request_data, *current_server);

// 					Reponse rep;
// 					std::string response = rep.handleRequest(request, *current_server);

// 					std::string http_status = "HTTP/1.1 200 OK\r\n";
// 					std::string full_response = http_status + response;
// 					send(fds[i].fd, full_response.c_str(), full_response.size(), 0);

// 					close(fds[i].fd);
// 					client_to_server.erase(fds[i].fd);
// 					fds.erase(fds.begin() + i);
// 					--i;
// 				}
// 			}
// 		}
// 	}
// }

// void Server::start()
// {
// 	bool is_running = true;
// 	std::vector<struct pollfd> fds;

// 	for (size_t i = 0; i < _ports.size(); i++)
// 	{
// 		int sock = socket(AF_INET, SOCK_STREAM, 0);
// 		if (sock == -1)
// 		{
// 			std::cerr << "Failed to create socket" << std::endl;
// 			continue;
// 		}

// 		int opt = 1;
// 		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

// 		struct sockaddr_in addr;
// 		addr.sin_family = AF_INET;
// 		addr.sin_port = htons(_ports[i]);
// 		addr.sin_addr.s_addr = INADDR_ANY;

// 		if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
// 		{
// 			std::cerr << "Failed to bind port " << _ports[i] << std::endl;
// 			close(sock);
// 			continue;
// 		}

// 		if (listen(sock, SOMAXCONN) == -1)
// 		{
// 			std::cerr << "Failed to listen on port " << _ports[i] << std::endl;
// 			close(sock);
// 			continue;
// 		}

// 		std::cout << "Server started on port " << _ports[i] << std::endl;

// 		struct pollfd server_pollfd = {sock, POLLIN, 0};
// 		fds.push_back(server_pollfd);
// 	}

// 	while (is_running)
// 	{
// 		int poll_count = poll(fds.data(), fds.size(), -1);
// 		if (poll_count == -1)
// 		{
// 			std::cerr << "Error : poll()" << std::endl;
// 			break;
// 		}

// 		for (size_t i = 0; i < fds.size(); i++)
// 		{
// 			if (fds[i].revents & POLLIN)
// 			{
// 				bool is_server_socket = false;
// 				for (size_t j = 0; j < _ports.size(); j++)
// 				{
// 					if (fds[i].fd >= 0 && fds[i].fd == fds[j].fd)
// 					{
// 						is_server_socket = true;
// 						break;
// 					}
// 				}

// 				if (is_server_socket)
// 				{
// 					struct sockaddr_in client_addr;
// 					socklen_t addrlen = sizeof(client_addr);
// 					int client_fd = accept(fds[i].fd, (struct sockaddr*)&client_addr, &addrlen);
// 					if (client_fd == -1)
// 					{
// 						std::cerr << "Failed to accept connection" << std::endl;
// 						continue;
// 					}
// 					struct pollfd client_pollfd = {client_fd, POLLIN, 0};
// 					fds.push_back(client_pollfd);
// 					std::cout << "New client connected: FD " << client_fd << std::endl;
// 				}
// 				else
// 				{
// 					char buffer[30720];
// 					int is_read = read(fds[i].fd, buffer, sizeof(buffer));
// 					if (is_read <= 0)
// 					{
// 						close(fds[i].fd);
// 						fds.erase(fds.begin() + i);
// 						--i;
// 						continue;
// 					}

// 					std::string request_data(buffer, is_read);
// 					Request request;
// 					request.parse(request_data, *this);

// 					Reponse rep;
// 					std::string reponse = rep.handleRequest(request, *this);
// 					std::string http_status = "HTTP/1.1 200 OK\r\n";
// 					std::string full_response = http_status + reponse;
// 					send(fds[i].fd, full_response.c_str(), full_response.size(), 0);
// 					close(fds[i].fd);
// 					fds.erase(fds.begin() + i);
// 					--i;
// 				}
// 			}
// 		}
// 	}
// 	stop();
// }


void Server::start()
{
	bool is_running = true;
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd == -1) {
		std::cerr << "Failed to create socket" << std::endl;
		return;
	}

	int opt = 1;
	setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	_address.sin_family = AF_INET;
	_address.sin_port = htons(listen_port);
	_address.sin_addr.s_addr = INADDR_ANY;

	if (bind(_server_fd, (struct sockaddr*)&_address, sizeof(_address)) == -1) {
		std::cerr << "Failed to bind socket" << std::endl;
		return;
	}

	if (listen(_server_fd, SOMAXCONN) == -1) {
		std::cerr << "Failed to listen on socket" << std::endl;
		return;
	}

	std::cout << "Server started on port " << listen_port << std::endl;

	std::vector<struct pollfd> fds;
	struct pollfd server_pollfd = {_server_fd, POLLIN, 0};
	fds.push_back(server_pollfd);

	while (is_running) {
		int poll_count = poll(fds.data(), fds.size(), 0);
		if (poll_count == -1) {
			std::cerr << "Error : poll()" << std::endl;
			break;
		}

		for (size_t i = 0; i < fds.size(); i++) {
			if (fds[i].revents & POLLIN) {
				if (fds[i].fd == _server_fd) {
					socklen_t addrlen = sizeof(_address);
					int client_fd = accept(_server_fd, (struct sockaddr*)&_address, &addrlen);
					if (client_fd == -1) {
						std::cerr << "Failed to accept connection" << std::endl;
						break;
					}

					struct pollfd client_pollfd = {client_fd, POLLIN, 100};
					fds.push_back(client_pollfd);
					std::cout << "New client connected: FD " << client_fd << std::endl;
				} else {
					char buffer[30720];
					std::string request_data;
					int is_read = read(fds[i].fd, buffer, sizeof(buffer));

					if (is_read < 0) {
						std::cerr << "Failed to read from socket" << std::endl;
						close(fds[i].fd);
						fds.erase(fds.begin() + i);
						--i;
						break;
					}

					request_data.append(buffer, is_read);

					size_t header_end = request_data.find("\r\n\r\n");
					if (header_end == std::string::npos) {
						continue;
					}

					size_t content_length = 0;
					size_t pos = request_data.find("Content-Length:");
					if (pos != std::string::npos) {
						size_t end = request_data.find("\r\n", pos);
						std::string len_str = request_data.substr(pos + 15, end - (pos + 15));
						content_length = atoi(len_str.c_str());
					}

					size_t body_start = header_end + 4;
					size_t body_received = request_data.size() - body_start;

					while (body_received < content_length) {
						is_read = read(fds[i].fd, buffer, sizeof(buffer));
						if (is_read <= 0) break;
						request_data.append(buffer, is_read);
						body_received = request_data.size() - body_start;
					}

					Request request;
					request.parse(request_data, *this);
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

int	Server::getClientMaxBodySize() const
{
	return (max_body_size);
}