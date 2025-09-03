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
		_statusCode = 200;
		_statusComment = "OK";
		_body = "<div class='result'>";
		_body += "<h2>Donnees GET recues :</h2>";
		
		std::istringstream iss(req.getQuerry_string());
		std::string token;
		while (std::getline(iss, token, '&'))
		{
			size_t eq = token.find('=');
			if (eq != std::string::npos)
			{
				std::string key = token.substr(0, eq);
				std::string val = token.substr(eq + 1);
				_body += "<p>" + key + ": " + val + "</p>";
			}
		}
		_body += "</div>";
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
	if ((true_path.find(".php") != std::string::npos || true_path.find(".py") != std::string::npos) 
		&& true_path.find("/cgi-bin/") != std::string::npos)
	{
		struct stat sb;
		if (stat(true_path.c_str(), &sb) != 0)
		{
			errorHandler(404);
			return;
		}

		std::string output = cgiExec(req, true_path);
		if (output == "504_GATEWAY_TIMEOUT")
		{
			errorHandler(504);
			return;
		}
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
			return;
		}
		else
		{
			errorHandler(500);
			return;
		}
	}
	if (req.getPath() == "/submit")
	{
		_statusCode = 200;
		_statusComment = "OK";
		
		std::istringstream iss(req.getBody());
		std::string token;
		while (std::getline(iss, token, '&'))
		{
			size_t eq = token.find('=');
			if (eq != std::string::npos)
			{
				std::string key = token.substr(0, eq);
				std::string val = token.substr(eq + 1);
				_body += "<p>" + key + ": " + val + "</p>";
			}
		}
		_headers["Content-Type"] = "text/html; charset=utf-8";
		return;
	}
	else
		errorHandler(404);
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
	}
}


std::string	Reponse::handleRequest(const Request &req, const Server &server)
{
	const Location* location = matchLocation(server, req.getPath());

	if (location && location->hasRedirect)
	{
		handleRedirect(location->redirectPath);
		std::ostringstream oss;
		oss << _body.size();
		_headers["Content-Length"] = oss.str();
		
		std::ostringstream ossCode;
		ossCode << _statusCode;
		std::string rep = req.getVersion() + " " + ossCode.str() + " " + _statusComment + "\r\n";
		for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
			rep += it->first + ": " + it->second + "\r\n";
		rep += "\r\n";
		rep += _body;
		return rep;
	}
	std::string true_path = findTruePath(server, location, req.getPath());
	if (req.getMethod() == "413")
		errorHandler(413);
	else if (!isMethodAllowed(location, req.getMethod()))
		errorHandler(405);
	else if (req.getMethod() == "GET")
		this->handleGET(req, true_path);
	else if (req.getMethod() == "POST")
		this->handlePOST(req, true_path);
	else if (req.getMethod() == "DELETE")
		this->handleDELETE(req, true_path);
	else
		errorHandler(501);

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
	size_t longest_match = 0;

	for (size_t i = 0; i < server.locations.size(); i++)
	{
		const Location &loc = server.locations[i];
		if (path == loc.path || 
			(path.find(loc.path) == 0 && ((loc.path.length() > 0 && loc.path[loc.path.length() - 1] == '/') || path[loc.path.length()] == '/')))
		{
			if (!match || loc.path.length() > longest_match)
			{
				match = &loc;
				longest_match = loc.path.length();
			}
		}
	}
	return match;
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
		return (true);

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
		_statusComment = "Method not allow";
		_body = readFile("var/www/error/405.html");
		if (_body.empty())
			_body = "<h1> 405 Method not allow </h1>";
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

	else if (error == 504)
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
	else if (error == 501)
	{
		_statusCode = 501;
		_statusComment = "Method not implemented";
		_body = readFile("var/www/error/501.html");
		if (_body.empty())
			_body = "<h1> 501 Method not implemented </h1>";
		_headers["Content-Type"] = "text/plain";
	}
	 else if (error == 403)
	{
		_statusCode = 403;
		_statusComment = "Forbidden";
		_body = readFile("var/www/error/403.html");
		if (_body.empty())
			_body = "<h1> 403 Forbidden </h1>";
		_headers["Content-Type"] = "text/plain";
		return;
	}
}

void Reponse::handleRedirect(const std::string& newPath)
{
	_statusCode = 301;
	_statusComment = "Moved Permanently";
	_headers["Location"] = newPath;
	_headers["Content-Type"] = "text/html";
	_body = "<html><head><meta http-equiv='refresh' content='0;url=" + newPath + "'></head>"
			"<body>Redirecting to <a href='" + newPath + "'>" + newPath + "</a></body></html>";
}