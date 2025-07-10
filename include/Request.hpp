#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <map>

# include "Server.hpp"

class Request
{
	private :
	
		std::string	_method;
		std::string	_path;
		std::string	_version;
		std::string	_querry_string;
		std::string	_body;
		std::map<std::string, std::string> _headers;
	public :
		Request();
		~Request();

		std::string	getMethod() const;
		std::string	getPath() const;
		std::string	getVersion() const;
		std::string	getQuerry_string() const;
		std::string	getBody() const;
		std::map<std::string, std::string>	getHeaders() const;

		void		parse(const std::string &raw_request);
		void		methodParse(const std::string &line);
		void		headersParse(const std::string &line);
		void		printAllToTest();
};

#endif