#include "Reponse.hpp"

Reponse::Reponse()
{

}

Reponse::~Reponse()
{

}

std::string	Reponse::handleRequest(const Request &req)
{
	if (req.getMethod() == "GET") 
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
			_statusCode = 404;
			_statusComment = "Not Found";
			_body = "<h1>404 Not Found</h1>";
			_headers["Content-Type"] = "text/html";
		}
	}
	else
	{
		_statusCode = 405;
		_statusComment = "Method Not Allowed";
		_body = "Unsuported Method";
		_headers["Content-Type"] = "text/plain";
	}
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