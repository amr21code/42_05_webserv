/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   opend: 2022/09/20 13:57:50 by anruland          #+#    #+#             */
/*   Updated: 2022/09/20 14:14:57 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpServer.class.hpp"
#include "httpConfig.class.hpp"

// Default constructor
httpServer::httpServer(void)
{
	if (DEBUG > 2)
		std::cout << "httpServer default constructor" << std::endl;
	// readConfig();
	this->mSockAddr.sin_family = this->mcConfDomain;
	this->mSockAddr.sin_port = htons(2000);
	this->mSockAddr.sin_addr.s_addr = INADDR_ANY;
	memset(this->mSockAddr.sin_zero, '\0', sizeof this->mSockAddr.sin_zero);
	this->openSocket();
	this->listenSocket();
} // CHECK THAT THIS WONT FAIL US!

// Constructor with valid path
httpServer::httpServer(std::string configPath)
{
	if (DEBUG > 2)
		std::cout << "httpServer constructor with path" << configPath << std::endl;
	this->mSockAddr.sin_family = this->mcConfDomain;
	this->mSockAddr.sin_addr.s_addr = INADDR_ANY;
	memset(this->mSockAddr.sin_zero, '\0', sizeof this->mSockAddr.sin_zero);
	this->openSocket();
}

// Constructor with valid path
httpServer::httpServer(httpConfig *config)
{
	if (DEBUG > 2)
		std::cout << "httpServer constructor with path" <<  std::endl;
	this->mConfig = config;
	this->mRespCode = "200 OK";
	this->mSockAddr.sin_family = this->mcConfDomain;
	this->mSockAddr.sin_port = htons(this->mConfig->getPort());
	this->mSockAddr.sin_addr.s_addr = inet_addr(this->mConfig->getHost().c_str());
	memset(this->mSockAddr.sin_zero, '\0', sizeof this->mSockAddr.sin_zero);
	try
	{
		this->openSocket();
	}
	catch(const std::logic_error& e)
	{
		throw e;
	}
	// std::vector<std::map<std::string, std::string> > temp = config->getConfLocations();
	// std::vector<std::map<std::string, std::string> >::iterator it = temp.begin();
	// std::map<std::string, std::string>::iterator ite = it->begin();
	// for (; it != temp.end(); it++)
	// {
	// 	std::cout << "--- new ---" << std::endl;
	// 	for (ite = it->begin(); ite != it->end() ; ite++)
	// 	{
	// 		std::cout << ite->first << " -> " << ite->second << std::endl;
	// 	}
	// }

}

httpServer::~httpServer(void)
{
    if (DEBUG > 2)
		std::cout << "httpServer destructor" << std::endl;
	this->closeSocket();
}

void httpServer::openSocket(void)
{
	if (DEBUG > 2)
		std::cout << "httpServer openSocket" << std::endl;
	this->mSocket = socket(this->mcConfDomain, this->mcConfComType, this->mcConfProtocol);
	if (this->mSocket < 0)
		throw std::logic_error("Error: cannot open socket");
	if (bind(this->mSocket, (struct sockaddr *)&this->mSockAddr, sizeof(this->mSockAddr)))
	{
		close(this->mSocket);
		throw std::logic_error("Error: cannot bind socket");
	}
}

void httpServer::closeSocket(void)
{
    if (DEBUG > 2)
		std::cout << "httpServer closeSocket" << std::endl;
	if (this->mSocket >= 0)
		close(this->mSocket);
}

void httpServer::log(std::string &message) const
{
    if (DEBUG > 2)
		std::cout << "httpServer log" << std::endl;
    std::cout << message << std::endl;
}

void	httpServer::listenSocket(void)
{
	if (DEBUG > 2)
		std::cout << "httpServer listenSocket()" << std::endl;
	// int		addrlen = sizeof(this->mSockAddr);
	// char	buffer[this->mcConfBufSize];
	// int		recv_return = 1;

	if (listen(this->mSocket, 30000))
	{
		std::cerr << "Error: listen() failed" << std::endl;
		return ;
	}
	this->announce();
	// while (1)
	// {
  		// bzero(buffer, this->mcConfBufSize);

  	// }
}

void	httpServer::receive(void)
{
    if (DEBUG > 2)
		std::cout << "httpServer receive" << std::endl;
	int		addrlen = sizeof(this->mSockAddr);
	std::vector<char> buffer(mcConfBufSize + 1, '\0');
	int		recv_return = 1;

	this->mMsgFD = accept(this->mSocket, (struct sockaddr *)&this->mSockAddr, (socklen_t *)&addrlen);
	if (this->mMsgFD < 0)
	{
		std::cerr << "Error: accept() failed" << std::endl;
		return ;
	}
	this->mIncMsg = "";
	// usleep(1000);
	while (this->mIncMsg.size() == 0)
	{
		while ((recv_return = recv(this->mMsgFD, buffer.data(), this->mcConfBufSize, MSG_DONTWAIT)) > 0)
		{
			if (DEBUG > 2)
				std::cout << "httpServer received something" << std::endl;
			//std::cout << "i" << this->mIncMsg << std::endl;
			//std::cout << "b" << buffer.data() << std::endl;
			if (this->mIncMsg.size() == 0)
				this->mIncMsg = buffer.data();
			else
				this->mIncMsg.append(buffer.data());
			buffer.assign(this->mcConfBufSize + 1, '\0');
			//bzero(buffer, this->mcConfBufSize);
			// std::cout << i << " " <<  this->mIncMsg << std::endl;
			// i++;
			// std::cout << "test" << std::endl;
		}
	}
	if (this->mIncMsg.size() > 0)
	{
		try
		{
			// std::cout << "try" << std::endl;
			this->mRequest = new httpRequest(this->mIncMsg, *this->mConfig);
			this->answer();
		}
		catch(const std::exception& e)
		{
			// std::cerr << e.what() << '\n';
			this->errorHandler(e.what());
		}
		
	}
	// std::cout << "TEST" << std::endl;
	else
	{
		this->errorHandler("400bad_request.html");
	} 
}


std::string httpServer::IntToString(size_t a)
{
    std::ostringstream temp;
    temp << a;
    return temp.str();
}

void	httpServer::generateResponse(size_t fileSize)
{
	if (DEBUG > 2)
		std::cout << "httpServer generateResponse" << std::endl;
	char buf[100];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	std::ifstream 	ifile;
	struct stat fileStats;
	timespec	modTime;
	// if (stat("test.html", &fileStats) == 0)
	if (stat(this->mRequest->getResource().c_str(), &fileStats) == 0)
		modTime = fileStats.st_mtim;
	ifile.open(this->mRequest->getResource().c_str());
	strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
	this->mResponse = "HTTP/1.1 ";
	this->mResponse.append(this->mRespCode);
	this->mResponse.append("\r\nDate: ");
	this->mResponse.append(buf);
	this->mResponse.append("\r\nServer: WebSurfer/0.1.2 (Linux)");
	if (fileSize > 0)
	{
		this->mResponse.append("\r\nLast-Modified: ");
		bzero(buf, 100);
		strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", gmtime(&modTime.tv_sec));
		this->mResponse.append(buf);
		this->mResponse.append("\r\nContent-Length: ");
		this->mResponse.append(this->IntToString(fileSize));
		this->mResponse.append("\r\nContent-Type: text/html");
	}
	this->mResponse.append("\r\nConnection: close\r\n\r\n");
}

void	httpServer::answer(void)
{
	if (DEBUG > 2)
		std::cout << "httpServer answer" << std::endl;
	std::ifstream 	ifile;
	std::ofstream 	ofile;
	std::string		tmp;
	std::string		fileContent;
	std::string 	tmpFile;

	// if (!this->mRequest->getReqType().compare("POST"))
	// {
	// 	ifile.open(this->mRequest->getResource().c_str());
	// 	try
	// 	{
	// 		// if (ifs.peek() == std::ifstream::traits_type::eof())

	// 		if (!ifile.good())
	// 			throw std::logic_error("404 Not Found");
	// 	}
	// 	catch(const std::logic_error& e)
	// 	{
	// 		this->errorHandler(e.what());
	// 		return ;
	// 	}
	// 	if (!this->mRequest->getFileExt().compare("php") || !this->mRequest->getFileExt().compare("py"))
	// 	{
	// 		srand(time(NULL));
	// 		int	nb = rand();
	// 		pid_t pid = -1;
	// 		tmpFile = "/tmp/" + this->mRequest->getFileName();
	// 		tmpFile.append(ft_itoa(nb));
	// 		ifile.close();
	// 		int tempFd 	= open(tmpFile.c_str(), O_RDWR|O_CREAT, 0644);
	// 		pid = fork();
	// 		int status = 0;
	// 		std::string execution = this->mRequest->getResource();
	// 		this->mEnv = setEnv("");
	// 		char **args = setEnv("irgendwas"); // this->mRequest->getQuery()
	// 		std::cout << args[0] << std::endl;
	// 		try
	// 		{
	// 			if (pid == -1)
	// 				throw std::logic_error("500 Internal Server Error");
	// 			if(pid == 0)
	// 			{
	// 				dup2(tempFd, STDOUT_FILENO);
	// 				close(tempFd);
	// 				execve(execution.c_str(), args, this->mEnv);
	// 				exit(errno);
	// 			}
	// 			else if (waitpid(-1, &status, 0))
	// 			{
	// 				// std::cerr << WEXITSTATUS(status) << "status "<< status << std::endl;
	// 				// std::cout << this->mEnv[0] << " !!!!RALF!!!! " << this->mEnv[1] << std::endl;
	// 				// if (WIFEXITED(status) && WEXITSTATUS(status))
	// 				// 	throw std::logic_error("500 Internal Server Error");
	// 				ifile.open(tmpFile.c_str());
	// 			}
	// 		}
	// 		catch(const std::logic_error& e)
	// 		{
	// 			this->errorHandler(e.what());
	// 		}
			

	// 		/* 
	// 		(achtung leaking FDs)
	// 		*/
	// 	}
	// 	while (getline(ifile, tmp))
	// 	{
	// 		fileContent.append(tmp);
	// 		fileContent.append("\r\n");
	// 	}
	// 	ifile.close();
	// 	if (tmpFile.size() > 0)
	// 		remove(tmpFile.c_str());
	// 	this->generateResponse(fileContent.size());
	// 	this->mResponse.append(fileContent);
	// }
	if (this->mRequest->getDirListing() && !this->mRequest->getReqType().compare("GET"))
	{
		ifile.open("./www/dirlisting.html");
		while (getline(ifile, tmp))
		{
			fileContent.append(tmp);
			fileContent.append("\r\n");
		}
		ifile.close();
    	std::string path = this->mRequest->getResource();
		DIR *dir;
		struct dirent *ent;
		std::string content;
		if ((dir = opendir (path.c_str())) != NULL) {
  		while ((ent = readdir (dir)) != NULL) {
			content = "<tr><td>";
			content.append(ent->d_name);
			content.append("</td></tr>\r\n");
			fileContent.append(content);
			// fileContent.append("\r\n");
  		}
  			closedir (dir);
		} else {
  		/* could not open directory */
  			perror ("error");
		}
		fileContent.append("</table></body></html>");

		this->generateResponse(fileContent.size());
		this->mResponse.append(fileContent);
	}
	else if (!this->mRequest->getReqType().compare("GET") || !this->mRequest->getReqType().compare("POST"))
	{
		ifile.open(this->mRequest->getResource().c_str());
		try
		{
			// if (ifs.peek() == std::ifstream::traits_type::eof())

			if (!ifile.good())
				throw std::logic_error("404 Not Found");
		}
		catch(const std::logic_error& e)
		{
			this->errorHandler(e.what());
			return ;
		}
		if (!this->mRequest->getFileExt().compare("php") || !this->mRequest->getFileExt().compare("py"))
		{
			srand(time(NULL));
			int	nb = rand();
			pid_t pid = -1;
			tmpFile = "/tmp/" + this->mRequest->getFileName();
			tmpFile.append(ft_itoa(nb));
			ifile.close();
			int tempFd 	= open(tmpFile.c_str(), O_RDWR|O_CREAT, 0644);
			pid = fork();
			int status = 0;
			this->mEnv = setEnv("");
			char **args = setEnv(this->mRequest->getFileExt()); // this->mRequest->getQuery()
			try
			{
				if (pid == -1)
					throw std::logic_error("500 Internal Server Error");
				if(pid == 0)
				{
					dup2(tempFd, STDOUT_FILENO);
					close(tempFd);
					execve(args[0], args, this->mEnv);
					exit(errno);
				}
				else if (waitpid(-1, &status, 0))
				{
					close(tempFd);
					// std::cerr << WEXITSTATUS(status) << "status "<< status << std::endl;
					// std::cout << this->mEnv[0] << " !!!!RALF!!!! " << this->mEnv[1] << std::endl;
					if (WIFEXITED(status) && WEXITSTATUS(status))
						throw std::logic_error("500 Internal Server Error");
					ifile.open(tmpFile.c_str());
					getline(ifile, tmp);
				}
			}
			catch(const std::logic_error& e)
			{
				this->errorHandler(e.what());
			}
			
			/* 
			(achtung leaking FDs)
			*/
		}
		while (getline(ifile, tmp))
		{
			fileContent.append(tmp);
			fileContent.append("\r\n");
		}
		ifile.close();
		if (tmpFile.size() > 0)
			remove(tmpFile.c_str());
		this->generateResponse(fileContent.size());
		this->mResponse.append(fileContent);
	}
	else if (!this->mRequest->getReqType().compare("DELETE"))
	{
		ifile.open(this->mRequest->getResource().c_str());
		try
		{
			if (!ifile.good())
				throw std::logic_error("404 Not Found");
			ifile.close();
			remove(this->mRequest->getResource().c_str());
		}
		catch(const std::logic_error& e)
		{
			this->errorHandler(e.what());
			return ;
		}
		this->generateResponse(fileContent.size());
		this->mResponse.append(fileContent);
	}
	else
		throw std::logic_error("405 Method Not Allowed");
	send(this->mMsgFD, this->mResponse.c_str(), this->mResponse.size(), 0);
	close(this->mMsgFD);
}

void	httpServer::answer(std::string file)
{
	if (DEBUG > 2)
		std::cout << "httpServer answer with file" << std::endl;
	std::ifstream 	ifile;
	std::string		tmp;
	std::string		fileContent;
	this->mRequest = new httpRequest(file, *this->mConfig, 1);
	ifile.open(this->mRequest->getResource().c_str());
	if (!ifile.good())
	{
		ifile.close();
		this->mRequest->setResource(this->mConfig->getDefaultMap()["error_page"], file);
		ifile.open(this->mRequest->getResource().c_str());
	}
	while (getline(ifile, tmp))
	{
		fileContent.append(tmp);
		fileContent.append("\r\n");
	}
	this->generateResponse(fileContent.size());
	this->mResponse.append(fileContent);
	// std::cout << this->mResponse << std::endl;
	// möglicherweise gesendete bytes abgleichen mit den zu sendenden und ggf. senden wiederholen
	// flag MSG_DONTWAIT statt 0 und nach EAGAIN/EWOULDBLOCK abfragen
	send(this->mMsgFD, this->mResponse.c_str(), this->mResponse.size(), 0);
	close(this->mMsgFD);
	this->mRespCode = "200 OK";
}

void	httpServer::errorHandler(std::string error)
{
	if (DEBUG > 2)
		std::cout << "httpServer errorHandler" << std::endl;
	int	errorNo = 0;

	errorNo = atoi(error.c_str());
	this->mRespCode = error;
	switch (errorNo)
	{
		case 400:
			this->answer("400bad_request.html");
			break;
		case 401:
			this->answer("401unauthorized.html");
			break;
		case 402:
			this->answer("000default.html");
			break;
		case 403:
			this->answer("403forbidden.html");
			break;
		case 404:
			this->answer("404not_found.html");
			break;
		case 405:
			this->answer("405method_not_allowed.html");
			break;
		case 500:
			this->answer("500internal_server_error.html");
			break;
		case 505:
			this->answer("505httpvernotsupported.html");
			break;
		
		default:
			this->answer("000default.html");
	}
}

void	httpServer::announce(void) const
{
	std::cout << C_GREEN << "Server" << C_GREY << " ( " << this->mConfig->getHost() << ":";
	std::cout << this->mConfig->getPort() << " ) " << this->mConfig->getServerNames();
	std::cout << C_DEF << C_GREEN << " started" << C_DEF << std::endl;
}

int		httpServer::getMsgFD(void)
{
	return(this->mMsgFD);
}

int		httpServer::getSocket(void)
{
	return(this->mSocket);
}

char **httpServer::setEnv(std::string queryString)
{
	if (queryString.size() > 0)
	{
		std::string execution;
		if (queryString == "php")
			execution = "/usr/bin/php-cgi";
		else if (queryString == "py")
			execution = "/usr/bin/python3";
		char **args = new char*[3];

		args[0] = strdup(execution.c_str());
		// args[1] = strdup("/home/pi/projects/C05_webserv/42_05_webserv/www/html/phptest/test.php");
		args[1] = strdup(this->mRequest->getResource().c_str());
		// args[2] = strdup("PFA=foobar");
		// args[3] = strdup("PFA2=boofar");
		// args[0] = strdup("REQUEST_METHOD=POST");
		// args[1] = strdup("QUERY_STRING=PFA=foobar");
		// args[2] = strdup("PATH_INFO=/phptest/test.php");
		// args[3] = strdup("PATH_TRANSLATED=/home/pi/projects/C05_webserv/42_05_webserv/www/html/phptest/test.php");
		args[2] = NULL;
		return (args);
	}
	else
	{
		char **envp = new char*[5]();
		envp[0] = strdup(("REQUEST_METHOD=" + this->mRequest->getReqType()).c_str());
		if (this->mRequest->getReqType() == "POST")
			envp[1] = strdup(("QUERY_STRING=" + this->mRequest->getPayload()).c_str());
		else
			envp[1] = strdup(("QUERY_STRING=" + this->mRequest->getQuery()).c_str());
		envp[2] = strdup(("PATH_INFO=" + this->mRequest->getResource()).c_str());
		envp[3] = strdup(("PATH_TRANSLATED=" + this->mRequest->getResource()).c_str());
		// envp[3] = strdup("CONTENT_LENGTH=7");
		envp[4] = NULL;
		return (envp);	
	}
	//explode querystring '&'
	return (NULL);
}
