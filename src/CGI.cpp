#include "../include/CGI.hpp"

std::vector<char *>	handleEnvp(const Request &req)
{
	std::vector<std::string> env;
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SCRIPT_FILENAME=" + req.getPath());
	env.push_back("REQUEST_METHOD=" + req.getMethod());
	env.push_back("QUERY_STRING=" + req.getQuerry_string());
	std::ostringstream iss;
	iss << req.getBody().length();
	env.push_back("CONTENT_LENGTH=" + iss.str());
	env.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");
	env.push_back("REDIRECT_STATUS=200");
	std::vector<char *> envp;
	for (size_t i = 0; i < env.size(); i++)
		envp.push_back(const_cast<char *>(env[i].c_str()));
	envp.push_back(NULL);
	return (envp);
}

std::string	cgiExec(const Request &req, std::string script_path)
{
	if (req.getPath().find("/cgi-bin/", 0) != 0 || req.getPath().find(".py") == std::string::npos)
		return ("");
	int		stdin_pipe[2];
	int		stdout_pipe[2];

	if (pipe(stdin_pipe) == -1|| pipe(stdout_pipe) == -1)
		return ("");
	pid_t pid = fork();
	if (pid == -1)
		return ("");
	if (pid == 0)
	{
		dup2(stdin_pipe[0], STDIN_FILENO);
		dup2(stdout_pipe[1], STDOUT_FILENO);
		close(stdin_pipe[1]);
		close(stdout_pipe[0]);
		
		std::vector<char *>	env = handleEnvp(req);
		char *argv[] = { const_cast<char *>("python3"), const_cast<char *>(script_path.c_str()), NULL };
		execve("/usr/bin/python3", argv, &env[0]);
		
		exit(1);
	}
	else
	{
		close(stdin_pipe[0]);
		close(stdout_pipe[1]);
		if (req.getMethod() == "POST" && !req.getBody().empty())
			write(stdin_pipe[1], req.getBody().c_str(), req.getBody().length());
		close(stdin_pipe[1]);
		std::stringstream	output;
		char				buffer[1024];
		ssize_t				n;
		while ((n = read(stdout_pipe[0], buffer, sizeof(buffer))) > 0)
			output.write(buffer, n);
		close(stdout_pipe[0]);
		waitpid(pid, NULL, 0);
		return(output.str());
	}
}