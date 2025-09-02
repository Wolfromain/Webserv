#include "Server.hpp"
#include "Reponse.hpp"

static std::map<int, Server*> client_to_server;
static std::map<int, std::string> client_buffers;

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

void Server::initSocket()
{
	for (size_t i = 0; i < _ports.size(); i++)
	{
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1)
		{
			std::cerr << "Failed to create socket for port " << _ports[i] << std::endl;
			continue;
		}

		int opt = 1;
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		struct sockaddr_in addr;
		std::memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(_ports[i]);
		addr.sin_addr.s_addr = INADDR_ANY;

		if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		{
			std::cerr << "Failed to bind on port " << _ports[i] << std::endl;
			close(sock);
			continue;
		}

		if (listen(sock, SOMAXCONN) == -1)
		{
			std::cerr << "Failed to listen on port " << _ports[i] << std::endl;
			close(sock);
			continue;
		}
		
		std::cout << "Listening on port " << _ports[i] << std::endl;
		_listen_fd = (sock);
	}
}

bool Server::is_server_socket(int fd, std::vector<Server*>& servers)
{
	for (size_t s = 0; s < servers.size(); s++)
		if (fd == servers[s]->_listen_fd)
			return true;
	return false;
}

void Server::handle_new_connection(std::vector<struct pollfd>& fds, int server_fd, std::vector<Server*>& servers)
{
	struct sockaddr_in client_addr;
	socklen_t addrlen = sizeof(client_addr);
	
	int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
	if (client_fd == -1)
		return;

	// Set non-blocking
	int flags = fcntl(client_fd, F_GETFL, 0);
	fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

	// Find corresponding server
	for (size_t s = 0; s < servers.size(); s++)
	{
		if (server_fd == servers[s]->_listen_fd)
		{
			client_to_server[client_fd] = servers[s];
			break;
		}
	}

	struct pollfd client_pfd = { client_fd, POLLIN, 0 };
	fds.push_back(client_pfd);
}

bool Server::handle_client_data(std::vector<struct pollfd>& fds, size_t i)
{
	char buffer[30720];
	int bytes_read = recv(fds[i].fd, buffer, sizeof(buffer), 0);

	if (bytes_read == 0)
	{
		close(fds[i].fd);
		client_to_server.erase(fds[i].fd);
		client_buffers.erase(fds[i].fd);
		fds.erase(fds.begin() + i);
		return false;
	}
	else if (bytes_read < 0)
		return true;

	client_buffers[fds[i].fd].append(buffer, bytes_read);
	std::string& full_request = client_buffers[fds[i].fd];


	size_t header_end = full_request.find("\r\n\r\n");
	if (header_end == std::string::npos)
		return true;


	std::string method;
	size_t method_end = full_request.find(' ');
	if (method_end != std::string::npos)
		method = full_request.substr(0, method_end);

	// vérifier Content-Length
	size_t content_length = 0;
	size_t cl_pos = full_request.find("Content-Length:");
	if (cl_pos != std::string::npos)
	{
		cl_pos += 15;
		while (cl_pos < full_request.size() && isspace(full_request[cl_pos]))
			cl_pos++;
		content_length = std::atoi(full_request.c_str() + cl_pos);
	}

	if (method == "GET")
		content_length = 0;

	size_t total_needed = header_end + 4 + content_length;
	if (full_request.size() < total_needed)
		return true;

	Server* server = client_to_server[fds[i].fd];
	Request request;
	request.parse(full_request, *server);
	request.printAllToTest();

	Reponse rep;
	std::string response = rep.handleRequest(request, *server);

	send(fds[i].fd, response.c_str(), response.size(), 0);

	// nettoyage
	close(fds[i].fd);
	client_to_server.erase(fds[i].fd);
	client_buffers.erase(fds[i].fd);
	fds.erase(fds.begin() + i);

	return false;
}

void Server::runPollLoop(std::vector<Server*>& servers)
{
	std::vector<struct pollfd> fds;

	for (size_t s = 0; s < servers.size(); s++)
	{
		std::cout << "Adding server FD " << servers[s]->_listen_fd << " to poll list" << std::endl;
		std::cout << "Server name: " << servers[s]->server_name << std::endl;
		std::cout << "Listening on ports: ";
		for (size_t p = 0; p < servers[s]->_ports.size(); p++)
			std::cout << servers[s]->_ports[p] << " " << std::endl;
		struct pollfd pfd = { servers[s]->_listen_fd, POLLIN, 0 };
		fds.push_back(pfd);
	}

	while (g_running)
	{
		int poll_count = poll(fds.data(), fds.size(), 5000);
		if (poll_count == -1)
		{
			std::cout << "Error: poll()" << std::endl;
			break;
		}
		for (size_t i = 0; i < fds.size() && g_running; i++)
		{
			if (fds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
			{
				if (!is_server_socket(fds[i].fd, servers))
				{
					close(fds[i].fd);
					client_to_server.erase(fds[i].fd);
					fds.erase(fds.begin() + i);
					--i;
				}
				continue;
			}

			if (!(fds[i].revents & POLLIN))
				continue;

			if (is_server_socket(fds[i].fd, servers))
			{
				handle_new_connection(fds, fds[i].fd, servers);
				continue;
			}

			if (!handle_client_data(fds, i))
			{
				--i;
				continue;
			}
		}
	}
	for (size_t i = 0; i < fds.size(); i++)
	{
		close(fds[i].fd);
		if (client_to_server.find(fds[i].fd) != client_to_server.end())
			client_to_server.erase(fds[i].fd);
	}
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

const std::vector<int>& Server::getPorts() const
{
	return (_ports);
}

void Server::addPort(int port)
{
	_ports.push_back(port);
}
