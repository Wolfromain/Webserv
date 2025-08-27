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
	else if (path.find(".js") != std::string::npos)
		return "application/javascript";
	else if (path.find(".png") != std::string::npos)
		return "image/png";
	else if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos)
		return "image/jpeg";
	else if (path.find(".gif") != std::string::npos)
		return "image/gif";
	else if (path.find(".pdf") != std::string::npos)
		return "application/pdf";
	return "application/octet-stream";
}

void Reponse::handleGET(const Request &req, std::string true_path)
{
	if (req.getPath() == "/submit")
	{
		std::string filename;
		std::istringstream iss(req.getQuerry_string());
		std::string token;
		while (std::getline(iss, token, '&'))
		{
			size_t eq = token.find('=');
			if (eq != std::string::npos)
			{
				std::string key = token.substr(0, eq);
				std::string val = token.substr(eq + 1);
				if (key == "filename")
					filename = val;
			}
		}

		std::string file_path = "var/www/" + filename;
		struct stat sb;
		if (stat(file_path.c_str(), &sb) != 0)
			errorHandler(404);

		std::string file_content = readFile(file_path);
		_body = "<h2>Resultat GET</h2>";
		_body += "<h3>Contenu du fichier " + filename + " :</h3><p>" + file_content + "</p>";
		_statusCode = 200;
		_statusComment = "OK";
		_headers["Content-Type"] = "text/html; charset=utf-8";
		return;
	}

	if ((true_path.find(".php") != std::string::npos || true_path.find(".py") != std::string::npos) && true_path.find("/cgi-bin/") != std::string::npos)
	{
		struct stat sb;
		if (stat(true_path.c_str(), &sb) != 0)
			errorHandler(404);
		else
		{
			std::string output = cgiExec(req, true_path); 
			if (output == "504_GATEWAY_TIMEOUT")
				errorHandler(504);
			else if (!output.empty())
			{
				std::string headers_part, body_part;
				size_t pos = output.find("\r\n\r\n");
				if (pos != std::string::npos)
				{
					headers_part = output.substr(0, pos);
					body_part = output.substr(pos + 4);

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

					_statusCode = 200;
					_statusComment = "OK";
					_body = body_part;
					_headers = headers;
				}
				else
				{
					_statusCode = 200;
					_statusComment = "OK";
					_body = output;
					_headers["Content-Type"] = "text/html";
				}
			}
			else
				errorHandler(500);
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
		errorHandler(404);
}


void	Reponse::handlePOST(const Request &req, std::string true_path)
{
	if ((true_path.find(".php") != std::string::npos || true_path.find(".py") != std::string::npos) && true_path.find("/cgi-bin/") != std::string::npos)
	{
		struct stat sb;
		if (stat(true_path.c_str(), &sb) != 0)
			errorHandler(404);
		else
		{
			std::string output = cgiExec(req, true_path);
			if (output == "504_GATEWAY_TIMEOUT")
				errorHandler(504);
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
	if (!req.getBody().empty() && true_path.find("/cgi-bin/") == std::string::npos)
	{
		if (true_path[true_path.length() - 1] == '/')
		{
			std::string tmpPath = "index.html" + true_path;
			true_path = tmpPath;
		}

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
			errorHandler(404);
	}
}

void	Reponse::handleDELETE(const Request &req, std::string true_path)
{
	if ((true_path.find(".php") != std::string::npos || true_path.find(".py") != std::string::npos) && true_path.find("/cgi-bin/") != std::string::npos)
	{
		struct stat sb;
		if (stat(true_path.c_str(), &sb) != 0)
			errorHandler(404);

		std::string output = cgiExec(req, true_path);
		if (output == "504_GATEWAY_TIMEOUT")
			errorHandler(504);
		_statusCode = 200;
		_statusComment = "OK";
		_body = output;
		_headers["Content-Type"] = "text/html; charset=utf-8";
	}
	else
	{
		if (remove(true_path.c_str()) == 0)
		{
			_statusCode = 204;
			_statusComment = "OK";
			_body = "File deleted";
		}
		else
			errorHandler(404);
		_headers["Content-Type"] = "text/plain";
	}
}

void	Reponse::handleNoMethod()
{
		_statusCode = 405;
		_statusComment = "Method Not Allowed";
		_body = readFile("var/www/error/405.html");
		if (_body.empty())
			_body = "<h1> 405 Method Not Allowed </h1>";
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
		this->handleDELETE(req, true_path);
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

void Reponse::errorHandler(int error)
{
	if (error == 404)
	{
		_statusCode = 404;
		_statusComment = "Not Found";
		_body = readFile("var/www/error/404.html");
		if (_body.empty())
			_body = "<h1> 404 Not Found </h1>";
		return;
	}

	else if (error == 405)
	{
		_statusCode = 405;
		_statusComment = "Method Not Allowed";
		_body = readFile("var/www/error/405.html");
		if (_body.empty())
			_body = "<h1> 405 Method Not Allowed </h1>";
		_headers["Content-Type"] = "text/plain";
		return;
	}

	else if (error == 500)
	{
		_statusCode = 500;
		_statusComment = "Internal Server Error";
		_body = readFile("var/www/error/500.html");
		if (_body.empty())
			_body = "<h1> 500 Internal Server Error </h1>";
		_headers["Content-Type"] = "text/html";
		return;
	}

	else if (error == 501)
	{
		_statusCode = 504;
		_statusComment = "Gateway Timeout";
		_body = readFile("var/www/error/504.html");
		if (_body.empty())
			_body = "<h1> 504 Gateway Timeout </h1>";
		_headers["Content-Type"] = "text/html";
		return;
	}

	else if (error == 413)
	{
		_statusCode = 413;
		_statusComment = "Payload Too Large";
		_body = readFile("var/www/error/413.html");
		if (_body.empty())
			_body = "<h1> 413 Payload Too Large </h1>";
		_headers["Content-Type"] = "text/html";
		return;
	}
}