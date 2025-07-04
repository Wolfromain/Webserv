#include "Reponse.hpp"

Reponse::Reponse()
{

}

Reponse::~Reponse()
{

}

std::string	readFile(const std::string &path)
{
	std::ifstream file(path.c_str());
	if (!file.is_open())
		return "";
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
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
		
}

void	Reponse::handleDELETE(const Request &req)
{

}

void	Reponse::handleNoMethod(const Request &req)
{
		_statusCode = 405;
		_statusComment = "Method Not Allowed";
		_body = "Unsuported Method";
		_headers["Content-Type"] = "text/plain";
}

std::string	Reponse::handleRequest(const Request &req)
{
	if (req.getMethod() == "GET") 
		this->handleGET(req);
	else
		this->handleNoMethod(req);
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