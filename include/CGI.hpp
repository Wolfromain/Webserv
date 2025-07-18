#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include <string>
# include <cstdlib>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <cstring>
# include <sys/stat.h>
# include <vector>

# include "Server.hpp"
# include "Request.hpp"

class Request;

std::vector<char *>	handleEnvp(const Request &req);
std::string			cgiExec(const Request &req, std::string script_path);

#endif