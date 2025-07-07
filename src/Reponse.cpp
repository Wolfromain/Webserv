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

std::string	Reponse::handleRequest(const Request &req)
{
	if (req.getMethod() == "GET")
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