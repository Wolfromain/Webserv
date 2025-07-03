#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <map>

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

		void	parse(const std::string &raw_request);
		void	methodParse(const std::string &line);
		void	headersParse(const std::string &line);
		void	printAllToTest();
};

#endif