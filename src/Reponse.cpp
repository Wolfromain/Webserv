#include "Reponse.hpp"

Reponse::Reponse()
{

}

Reponse::~Reponse()
{

}

void	Reponse::handleGET(const Request &req)
{
	if (req.getPath() == "/") 
	{
		_statusCode = 200;
		_statusComment = "OK";
		_body = "<html><h1>Bienvenue</h1></html>"; //page d'acueil a setup
		_headers["Content-Type"] = "text/html";
	}
	else
	{
		std::string buffer = readFile("www" + req.getPath());
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

void	Reponse::handlePOST(const Request &req)
{
	//call cgi or whatever for body
	_statusCode = 200;
	_statusComment = "OK";
	_body = "<h1>POST reçu</h1><pre>" + req.getBody() + "</pre>";
	_headers["Content-Type"] = "text/html";
}

void	Reponse::handleDELETE(const Request &req)
{
	if (remove(("www" + req.getPath()).c_str()) == 0)
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

	if (!isMethodAllowed(location, req.getMethod()))
		this->handleNoMethod();
	else if (req.getMethod() == "GET")
		this->handleGET(req);
	else if (req.getMethod() == "POST")
		this->handlePOST(req);
	else if (req.getMethod() == "DELETE")
		this->handleDELETE(req);
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

	if (location)
		relativePath = path.substr(location->path.length());

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
