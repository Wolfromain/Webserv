#ifndef REPONSE_HPP
# define REPONSE_HPP

# include <iostream>
# include <sstream>
# include <cstdlib>
# include <map>

# include "Server.hpp"
# include "CGI.hpp"

class Request;
class Server;
struct Location;

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
		std::string getContentType(const std::string &path);
		std::string	handleRequest(const Request &req, const Server &server);
		void		handleGET(const Request &req, std::string true_path);
		void		handlePOST(const Request &req, std::string true_path);
		void		handleDELETE(const Request &req, std::string true_path);
		void		handleRedirect(const std::string& newPath);


		const Location* matchLocation(const Server &server, const std::string &path);
		std::string findTruePath(const Server &server, const Location *location, const std::string &path);
		bool isMethodAllowed(const Location *location, const std::string &method);
		void errorHandler(int error);
};

#endif