#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <map>

class Request
{
	private :
		std::string	_method;
		std::string	_path;
		std::string	_body;
		std::map<std::string, std::string> headers;
	public :
		Request();
		~Request();

		void	parse(const std::string &raw_request);
};

#endif