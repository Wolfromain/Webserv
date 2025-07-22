#ifndef REPONSE_HPP
#define REPONSE_HPP

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <map>

#include "Server.hpp"
#include "CGI.hpp"

class Request;
class Server;

class Reponse
{
	private :
		int									_statusCode;
		std::string							_statusComment;
		std::map<std::string, std::string>	_headers;
		std::string							_body;

	public :
		Reponse();
		~Reponse();
		// std::string	handleRequest(const Request &req);
		std::string getContentType(const std::string &path);
		std::string	handleRequest(const Request &req, const Server &server);
		void		handleGET(const Request &req, std::string true_path);
		void		handlePOST(const Request &req, std::string true_path);
		void		handleDELETE(std::string true_path);
		void		handleNoMethod();

		//Locations
		const Location* matchLocation(const Server &server, const std::string &path);
		std::string findTruePath(const Server &server, const Location *location, const std::string &path);
		bool isMethodAllowed(const Location *location, const std::string &method);


};

#endif