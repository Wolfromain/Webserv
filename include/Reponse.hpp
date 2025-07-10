#ifndef REPONSE_HPP
#define REPONSE_HPP

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <map>

#include "Server.hpp"

class Request;

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
		std::string	handleRequest(const Request &req);
		void		handleGET(const Request &req);
		void		handlePOST(const Request &req);
		void		handleDELETE(const Request &req);
		void		handleNoMethod();

};

#endif