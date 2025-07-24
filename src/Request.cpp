#include "../include/Request.hpp"

Request::Request()
{

}

Request::~Request()
{

}


std::string	Request::getMethod() const
{
	return (_method);

}

std::string	Request::getPath() const
{
	return (_path);

}

std::string	Request::getVersion() const
{
	return (_version);

}

std::string	Request::getQuerry_string() const
{
	return (_querry_string);

}

std::string	Request::getBody() const
{
	return (_body);
}

std::map<std::string, std::string>	Request::getHeaders() const
{
	return (_headers);
}

void	Request::methodParse(const std::string &line)
{
	std::istringstream iss(line);
	iss >> _method >> _path >> _version;
	std::size_t pos = _path.find('?');
	if (pos != std::string::npos)
	{
		_querry_string = _path.substr(pos + 1);
		_path = _path.substr(0, pos);
	}
}

void	Request::headersParse(const std::string &line)
{
	std::size_t pos = line.find(':');
	if (pos == std::string::npos)
		return;
	std::string key = line.substr(0, pos);
	std::string val = "";
	if (pos + 1 < line.size())
		val = line.substr(pos + 1);
	while (!val.empty() && val[0] == ' ')
		val = val.substr(1);
	while (!val.empty() && val[val.size() - 1] == ' ')
		val = val.substr(0, val.size() - 1); 
	_headers.insert(std::pair<std::string, std::string>(key, val));

}

void	Request::parse(const std::string &raw_requeste)
{
	std::istringstream iss(raw_requeste);
	std::string	line;
	if (std::getline(iss, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		methodParse(line);
	}
	while (std::getline(iss, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.empty())
			break;
		headersParse(line);
	}
	std::string	isContLen = _headers["Content-Length"];
	int	len = 0;
	if (!isContLen.empty())
		len = atoi(isContLen.c_str());
	if (len > 0)
	{
		for(int i = 0; i < len; i++)
		{
			char c;
			if (iss.get(c))
				_body.push_back(c);
			else
				break;
		}
	}
}

void	Request::printAllToTest()
{
	std::cout << "Methode :" << _method << std::endl;
	std::cout << "Path :" << _path << std::endl;
	std::cout << "Version :" << _version << std::endl;
	std::cout << "Querry string :" << _querry_string << std::endl;
	std::cout << "Body :" << _body << std::endl;
	std::cout << "Headers :" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
		std::cout << it->first << ": " << it->second << std::endl;
	std::cout << std::endl;
}