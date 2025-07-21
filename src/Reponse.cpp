#include "Reponse.hpp"

Reponse::Reponse()
{

}

Reponse::~Reponse()
{

}

void	Reponse::handleGET(std::string true_path)
{
	if (true_path == "/") 
	{
		_statusCode = 200;
		_statusComment = "OK";
		_body = readFile(true_path); //page d'acueil a setup
		_headers["Content-Type"] = "text/html";
	}
	else
	{
		std::string buffer = readFile(true_path);
		if (!buffer.empty())
		{
			_statusCode = 200;
			_statusComment = "OK";
			_body = buffer;
			_headers["Content-Type"] = "text/html";
		}
		else
		{
			_statusCode = 404;
			_statusComment = "Not Found";
			_body = "<h1>404 Not Found</h1>";
			_headers["Content-Type"] = "text/html";
		}
	}
}	

void	Reponse::handlePOST(const Request &req, std::string true_path)
{
	std::string output = cgiExec(req, true_path);
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

		if (line.find("Status:") == 0) {
			std::string status_line = line.substr(7);
			size_t space = status_line.find(' ');
			if (space != std::string::npos) {
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

	// _statusCode = status_code;
	// _statusComment = status_text;
	// _body = body_part;
	// _headers = headers;
}

void	Reponse::handleDELETE(std::string true_path)
{
	if (remove(true_path.c_str()) == 0)
	{
		_statusCode = 200;
		_statusComment = "OK";
		_body = "File deleted";
	}
	else
	{
		_statusCode = 404;
		_statusComment = "Not Found";
		_body = "Impossible to delete file";
	}
	_headers["Content-Type"] = "text/plain";
}

void	Reponse::handleNoMethod()
{
		_statusCode = 405;
		_statusComment = "Method Not Allowed";
		_body = "Unsuported Method";
		_headers["Content-Type"] = "text/plain";
}

// std::string	Reponse::handleRequest(const Request &req)
// {
// 	if (req.getMethod() == "GET")
// 		this->handleGET(req);
// 	else if (req.getMethod() == "POST")
// 		this->handlePOST(req);
// 	else if (req.getMethod() == "DELETE")
// 		this->handleDELETE(req);
// 	else
// 		this->handleNoMethod();
// 	std::ostringstream oss;
// 	oss << _body.size();
// 	_headers["Content-Length"] = oss.str();

// 	std::ostringstream ossCode;
// 	ossCode << _statusCode;
// 	std::string	rep = req.getVersion() + " " + ossCode.str() + " " + _statusComment + "\r\n";
// 	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
// 		rep += it->first + ": " + it->second + "\r\n";
// 	rep += "\r\n";
// 	rep += _body;
	
// 	return (rep);
// }


std::string	Reponse::handleRequest(const Request &req, const Server &server)
{
	const Location* location = matchLocation(server, req.getPath());

	std::string true_path = findTruePath(server, location, req.getPath());
	if (!isMethodAllowed(location, req.getMethod()))
		this->handleNoMethod();
	else if (req.getMethod() == "GET")
		this->handleGET(true_path);
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
	std::string relativePath = path;

	if (location && !location->root.empty())
		root = location->root;
	else
		root = server.root;

	if (location && path.length() > location->path.length())
		relativePath = path.substr(location->path.length());
	else if (location && path.length() == location->path.length())
		relativePath = "";
	else
		relativePath = path;

	if (!root.empty() && root[root.length() - 1] == '/')
		root.erase(root.length() - 1);

	if (!relativePath.empty() && relativePath[0] == '/')
		relativePath = relativePath.substr(1);

	return (root + "/" + relativePath);
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
