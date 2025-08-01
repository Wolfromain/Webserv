#include "Reponse.hpp"

Reponse::Reponse()
{

}

Reponse::~Reponse()
{

}

std::string Reponse::getContentType(const std::string &path)
{
	if (path.find(".html") != std::string::npos)
		return "text/html";
	else if (path.find(".css") != std::string::npos)
		return "text/css";
	else
		return "application/octet-stream";
}

void Reponse::handleGET(const Request &req, std::string true_path)
{
	if (true_path.find(".py") != std::string::npos && true_path.find("/cgi-bin/") != std::string::npos)
	{
		struct stat sb;
		if (stat(true_path.c_str(), &sb) != 0)
		{
			_statusCode = 404;
			_statusComment = "Not Found";
			_body = "<h1>404 Not Found</h1>";
			_headers["Content-Type"] = "text/html";
			return;
		}
		else
		{
			std::string output = cgiExec(req, true_path); 
			if (output == "504_GATEWAY_TIMEOUT")
			{
				_statusCode = 504;
				_statusComment = "Gateway Timeout";
				_body = "<h1>504 Gateway Timeout</h1>";
				_headers["Content-Type"] = "text/html";
				return;
			}
			else if (!output.empty())
			{
				_statusCode = 200;
				_statusComment = "OK";
				_body = output;
				_headers["Content-Type"] = "text/html";
			}
			else
			{
				_statusCode = 500;
				_statusComment = "Internal Server Error";
				_body = "<h1>500 Internal Server Error</h1>";
				_headers["Content-Type"] = "text/html";
			}
			return;
		}
	}

	if (true_path[true_path.length() - 1] == '/')
		true_path += "index.html";

	std::string buffer = readFile(true_path);
	if (!buffer.empty())
	{
		_statusCode = 200;
		_statusComment = "OK";
		_body = buffer;
		std::string contentType = getContentType(true_path);
		_headers["Content-Type"] = contentType;
	}
	else
	{
		_statusCode = 404;
		_statusComment = "Not Found";
		_body = "<h1>404 Not Found</h1>";
		_headers["Content-Type"] = "text/html";
	}
}


void	Reponse::handlePOST(const Request &req, std::string true_path)
{
	if (true_path.find(".py") != std::string::npos && true_path.find("/cgi-bin/") != std::string::npos)
	{
		struct stat sb;
		if (stat(true_path.c_str(), &sb) != 0)
		{
			_statusCode = 404;
			_statusComment = "Not Found";
			_body = "<h1>404 Not Found</h1>";
			_headers["Content-Type"] = "text/html";
			return;
		}
		else
		{
			std::string output = cgiExec(req, true_path);
			if (output == "504_GATEWAY_TIMEOUT")
			{
				_statusCode = 504;
				_statusComment = "Gateway Timeout";
				_body = "<h1>504 Gateway Timeout</h1>";
				_headers["Content-Type"] = "text/html";
				return;
			}
			std::string headers_part, body_part;
			size_t pos = output.find("\r\n\r\n");
			if (pos != std::string::npos)
			{
				headers_part = output.substr(0, pos);
				body_part = output.substr(pos + 4);
			}

			std::map<std::string, std::string> headers;
			std::istringstream stream(headers_part);
			std::string line;

			while (std::getline(stream, line))
			{
				if (line.size() >= 2 && line[line.size()-1] == '\r')
					line = line.substr(0, line.size()-1);

				size_t sep = line.find(": ");
				if (sep != std::string::npos)
				{
					std::string key = line.substr(0, sep);
					std::string value = line.substr(sep + 2);
					headers[key] = value;
				}
			}
			int status_code;
			std::string status_text = "OK";

			while (std::getline(stream, line))
			{
				if (line.size() >= 2 && line[line.size()-1] == '\r')
					line = line.substr(0, line.size()-1);

				if (line.find("Status:") == 0)
				{
					std::string status_line = line.substr(7);
					size_t space = status_line.find(' ');
					if (space != std::string::npos)
					{
						status_code = std::atoi(status_line.substr(0, space).c_str());
						status_text = status_line.substr(space + 1);
					}
					continue;
				}
				size_t sep = line.find(": ");
				if (sep != std::string::npos)
				{
					std::string key = line.substr(0, sep);
					std::string value = line.substr(sep + 2);
					headers[key] = value;
				}
			}
			_statusCode = status_code;
			_statusComment = status_text;
			_body = body_part;
			_headers = headers;
		}
	}
}

void	Reponse::handleDELETE(std::string true_path)
{
	if (remove(true_path.c_str()) == 0)
	{
		_statusCode = 204;
		_statusComment = "OK";
		_body = "File deleted";
	}
	else
	{
		_statusCode = 404;
		_statusComment = "Not Found";
		_body = "<h1>404 Not Found</h1>";
	}
	_headers["Content-Type"] = "text/plain";
}

void	Reponse::handleNoMethod()
{
		_statusCode = 405;
		_statusComment = "Method Not Allowed";
		_body = "<h1>405 Unsuported Method</h1>";
		_headers["Content-Type"] = "text/plain";
}

std::string	Reponse::handleRequest(const Request &req, const Server &server)
{
	const Location* location = matchLocation(server, req.getPath());

	std::string true_path = findTruePath(server, location, req.getPath());
	if (!isMethodAllowed(location, req.getMethod()))
		this->handleNoMethod();
	else if (req.getMethod() == "GET")
		this->handleGET(req, true_path);
	else if (req.getMethod() == "POST")
		this->handlePOST(req, true_path);
	else if (req.getMethod() == "DELETE")
		this->handleDELETE(true_path);
	else
		this->handleNoMethod();
	std::ostringstream oss;
	oss << _body.size();
	_headers["Content-Length"] = oss.str();

	std::ostringstream ossCode;
	ossCode << _statusCode;
	std::string	rep = req.getVersion() + " " + ossCode.str() + " " + _statusComment + "\r\n";
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
		rep += it->first + ": " + it->second + "\r\n";
	rep += "\r\n";
	rep += _body;
	
	return (rep);
}

const Location *Reponse::matchLocation(const Server &server, const std::string &path)
{
	const Location *match = NULL;

	for (size_t i = 0; i < server.locations.size(); i++)
	{
		const Location &loc = server.locations[i];
		if (path.find(loc.path) == 0)
		{
			if (!match || loc.path.length() > match->path.length())
				match = &loc;
		}
	}
	return (match);
}

std::string Reponse::findTruePath(const Server &server, const Location *location, const std::string &path)
{
	std::string root;
	if (location && !location->root.empty())
		root = location->root;
	else
		root = server.root;

	if (!root.empty() && root[root.size() - 1] == '/')
		root.erase(root.size() - 1);

	std::string relativePath = path;

	if (!relativePath.empty() && relativePath[0] == '/')
		relativePath = relativePath.substr(1);

	std::string fullPath = root + "/" + relativePath;

	if (location && (relativePath.empty() || fullPath[fullPath.size() - 1] == '/'))
	{
		if (!location->index.empty())
		{
			if (!relativePath.empty() && fullPath[fullPath.size() - 1] != '/')
				fullPath += "/";
			fullPath += location->index;
		}
	}
	return (fullPath);
}




bool Reponse::isMethodAllowed(const Location *location, const std::string &method)
{
	if (!location || location->allow_methods.empty())
		return (false);

	std::vector<std::string>::const_iterator it = location->allow_methods.begin();
	while (it != location->allow_methods.end())
	{
		if (*it == method)
			return (true);
		++it;
	}
	return (false);
}
