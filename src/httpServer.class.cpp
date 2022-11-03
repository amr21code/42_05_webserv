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
}

httpServer::~httpServer(void)
{
    if (DEBUG > 2)
		std::cout << "httpServer destructor" << std::endl;

	this->closeSocket();
}

/**
 * @brief opens and binds socket
 * 
 */
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

/**
 * @brief closes socket
 * 
 */
void httpServer::closeSocket(void)
{
    if (DEBUG > 2)
		std::cout << "httpServer closeSocket" << std::endl;

	if (this->mSocket >= 0)
		close(this->mSocket);
}

/**
 * @brief listens to socket
 * 
 */
void	httpServer::listenSocket(void)
{
	if (DEBUG > 2)
		std::cout << "httpServer listenSocket()" << std::endl;

	if (listen(this->mSocket, 30000))
	{
		std::cerr << "Error: listen() failed" << std::endl;
		return ;
	}
	this->announce();
}


int	httpServer::acceptSocket(void)
{
	if (DEBUG > 2)
		std::cout << "httpServer accept" << std::endl;

	int					addrlen = sizeof(this->mSockAddr);
	int					tmpfd = -1;

	tmpfd = accept(this->mSocket, (struct sockaddr *)&this->mSockAddr, (socklen_t *)&addrlen);
	if (tmpfd < 0)
	{
		std::cerr << "Error: accept() failed" << std::endl;
		return (-1);
	}
	return (tmpfd);
}

/**
 * @brief receives incomming message on socket 
 */
void	httpServer::receive(int fd)
{
    if (DEBUG > 2)
		std::cout << "httpServer receive" << std::endl;

	std::vector<char>	buffer(mcConfBufSize + 1, '\0');
	int					recv_return = 1;

	// tmpfd = accept(this->mSocket, (struct sockaddr *)&this->mSockAddr, (socklen_t *)&addrlen);
	// if (tmpfd < 0)
	// {
	// 	std::cerr << "Error: accept() failed" << std::endl;
	// 	return (-1);
	// }
	// while (tmpmsg.size() == 0)
	// {
		// usleep(100);
			// recv_return = recv(tmpfd, buffer.data(), this->mcConfBufSize, MSG_DONTWAIT);
		// while (recv_return > 0)
		// {
			recv_return = recv(fd, buffer.data(), this->mcConfBufSize, 0);
			if (!recv_return)
			{
				std::cerr << "Error: client closed connection" << recv_return << std::endl;
				close(fd);
				return ;
			}
			else if (recv_return < 0 && (static_cast<std::string>(buffer.data())).size() == 0)
			{
				std::cerr << "Error: recv() failed " << recv_return << std::endl;
				close(fd);
				return ;
			}
			if (DEBUG > 2)
				std::cout << "httpServer received something" << std::endl;
			if (this->mMsg[fd].size() == 0)
				this->mMsg[fd] = buffer.data();
			else
				this->mMsg[fd].append(buffer.data());
			// if (tmpmsg.size() == 0)
			// 	tmpmsg = buffer.data();
			// else
			// 	tmpmsg.append(buffer.data(), recv_return); 
			// buffer.assign(this->mcConfBufSize + 1, '\0');
			// usleep(100);
			// recv_return = recv(tmpfd, buffer.data(), this->mcConfBufSize, MSG_DONTWAIT);
	// 	}
	// }
	// this->mMsg[tmpfd] = tmpmsg;
	std::cout << "fd " << fd << " msg " << this->mMsg[fd] << std::endl;
}

/**
 * @brief handles file upload
 */
void	httpServer::fileUpload(int fd)
{
	std::map<std::string, std::string>	tmpRequest = this->mRequest[fd]->getRequest();
	size_t		 						length = atoi(tmpRequest["Content-Length"].c_str());
	std::string 						boundary = tmpRequest["Content-Type"].substr(tmpRequest["Content-Type"].find("=")+1);
	std::string							tmpPayload = this->mRequest[fd]->getPayload();
	size_t								payloadPos = 0;
	int									nbChunks = -1;
	size_t								lineEnd = 0;
	std::string							fileName;
	std::string							content;
	std::fstream						file;
	std::string							path;
	
	while (payloadPos != std::string::npos)
	{
		payloadPos = tmpPayload.find(boundary, payloadPos + boundary.size());
		nbChunks++;
	}

	payloadPos = tmpPayload.find(boundary);
	while (nbChunks >= 0)
	{
		payloadPos = payloadPos + boundary.size() + 2;
		if (tmpPayload.find("Content-Disposition") == payloadPos)
		{
			lineEnd = tmpPayload.find("\r\n", payloadPos);
			if ((payloadPos = tmpPayload.find("filename=", payloadPos)) < lineEnd)
			{
				fileName = tmpPayload.substr(payloadPos + 10, tmpPayload.find("\"", payloadPos + 10) - payloadPos - 10);
				std::cout << this->mConfig->getConfLocations()[this->mRequest[fd]->getLocNb()]["upload"] << std::endl;
				path = ((path.append(this->mConfig->getConfLocations()[this->mRequest[fd]->getLocNb()]["root"])).append(this->mConfig->getConfLocations()[this->mRequest[fd]->getLocNb()]["upload"])).append(fileName);
				file.open(path.c_str(), std::fstream::out | std::fstream::trunc);
				if (!file.good())
					throw std::logic_error("404 Not Found");
				payloadPos = tmpPayload.find("\r\n\r\n") + 4;
				content = tmpPayload.substr(payloadPos, tmpPayload.find(boundary, payloadPos) - payloadPos - 2);
				file.write(&content[0], content.size());
				file.close();
			}
			else
				payloadPos = tmpPayload.find("\r\n\r\n") + 4;
		}
		payloadPos = tmpPayload.find(boundary, payloadPos);
		nbChunks--;
		if (length == tmpPayload.size())
			throw std::logic_error("201 Created");
		else
			throw std::logic_error("500 Internal Server Error");
	}
}


std::string httpServer::IntToString(size_t a)
{
    std::ostringstream temp;
    temp << a;
    return temp.str();
}

/**
 * @brief creates response including header to incoming request

 */
void	httpServer::generateResponse(int fd, size_t fileSize)
{
	if (DEBUG > 2)
		std::cout << "httpServer generateResponse" << std::endl;

	std::vector<char>	buf(this->mcConfBufSize + 1, '\0');
	time_t				now = time(0);
	struct tm			tm = *gmtime(&now);
	std::ifstream 		ifile;
	struct stat			fileStats;
	timespec			modTime;

	if (stat(this->mRequest[fd]->getResource().c_str(), &fileStats) == 0)
		modTime = fileStats.st_mtim;
	ifile.open(this->mRequest[fd]->getResource().c_str());
	strftime(buf.data(), this->mcConfBufSize, "%a, %d %b %Y %H:%M:%S %Z", &tm);
	this->mResponse = "HTTP/1.1 ";
	this->mResponse.append(this->mRespCode);
	if (this->mRequest[fd]->getRedirect())
	{
		this->mResponse.append("\r\nLocation: ");
		this->mResponse.append(this->mRequest[fd]->getResource());
	}
	this->mResponse.append("\r\nDate: ");
	this->mResponse.append(buf.data());
	this->mResponse.append("\r\nServer: WebSurfer/0.1.2 (Linux)");
	if (fileSize > 0)
	{
		if (!this->mRequest[fd]->getDirListing())
		{
			this->mResponse.append("\r\nLast-Modified: ");
			buf.assign(this->mcConfBufSize + 1, '\0');
			strftime(buf.data(), this->mcConfBufSize, "%a, %d %b %Y %H:%M:%S %Z", gmtime(&modTime.tv_sec));
			this->mResponse.append(buf.data());
		}
		this->mResponse.append("\r\nContent-Length: ");
		this->mResponse.append(this->IntToString(fileSize));
		this->mResponse.append("\r\nContent-Type: text/html");
	}
	this->mResponse.append("\r\nConnection: close\r\n\r\n");
}

/**
 * @brief sends response 
 * 
 * @param fd 
 */
void	httpServer::answer(int fd)
{
	std::ifstream 	ifile;
	std::string		tmp;
	std::string		fileContent;
	std::string 	tmpFile;

	if (DEBUG > 2)
		std::cout << "httpServer answer" << std::endl;
	if (this->mMsg[fd].size() > 0)
	{
		try
		{
			// this->mRequest[fd] = new httpRequest(this->mMsg[fd], *this->mConfig);
			if (this->mRequest[fd]->getReqType() == "POST" && !this->mRequest[fd]->getRequest()["Content-Type"].find(" multipart/form-data; boundary="))
				this->fileUpload(fd);
		}
		catch(const std::exception& e)
		{
			this->errorHandler(fd, e.what());
		}
	}
	else
	{
		this->errorHandler(fd, "400 Bad Request");
	} 
	if (this->mRequest[fd]->getRedirect())
	{
		this->mRespCode = "301 Moved Permanently";
		this->generateResponse(fd, 0);
	}
	else if (this->mRequest[fd]->getDirListing() && this->mRequest[fd]->getReqType() == "GET")
	{
		try 
		{
			handleDirListing(fd);
		}
		catch(const std::logic_error &e)
		{
			this->errorHandler(fd, e.what());
			return ;
		}
	}
	else if (this->mRequest[fd]->getReqType() == "GET" || this->mRequest[fd]->getReqType() == "POST")
	{
		ifile.open(this->mRequest[fd]->getResource().c_str());
		try
		{
			if (!ifile.good())
				throw std::logic_error("404 Not Found");
		}
		catch(const std::logic_error& e)
		{
			this->errorHandler(fd, e.what());
			return ;
		}
		if (!this->mRequest[fd]->getFileExt().compare("php") || !this->mRequest[fd]->getFileExt().compare("py") || !this->mRequest[fd]->getFileExt().compare("pl"))
		{
			tmpFile = handleCGI(fd);
			ifile.close();
			ifile.open(tmpFile.c_str());
			if (!this->mRequest[fd]->getFileExt().compare("php"))
				getline(ifile, tmp);
		}
		while (getline(ifile, tmp))
		{
			fileContent.append(tmp);
			fileContent.append("\r\n");
		}
		ifile.close();
		if (tmpFile.size() > 0)
			remove(tmpFile.c_str());
		this->generateResponse(fd, fileContent.size());
		this->mResponse.append(fileContent);
	}
	else if (this->mRequest[fd]->getReqType() == "DELETE")
	{
		ifile.open(this->mRequest[fd]->getResource().c_str());
		try
		{
			if (!ifile.good())
				throw std::logic_error("404 Not Found");
			ifile.close();
			remove(this->mRequest[fd]->getResource().c_str());
		}
		catch(const std::logic_error& e)
		{
			this->errorHandler(fd, e.what());
			return ;
		}
		this->generateResponse(fd, fileContent.size());
		this->mResponse.append(fileContent);
	}
	else
	{
		this->errorHandler(fd, "405 Method Not Allowed");
		return ;
	}
	// std::cout << "before send" << std::endl;
	send(fd, this->mResponse.c_str(), this->mResponse.size(), 0);
	// std::cout << "after send" << std::endl;
	// if send fails -> remove msgfd from epoll
	// delete this->mRequest;
}

/**
 * @brief sends error response
 */
void	httpServer::answer(int fd, std::string file)
{
	if (DEBUG > 2)
		std::cout << "httpServer answer with file" << std::endl;
	std::ifstream 	ifile;
	std::string		tmp;
	std::string		fileContent;
	this->mRequest[fd] = new httpRequest(file, *this->mConfig, 1);
	ifile.open(this->mRequest[fd]->getResource().c_str());
	if (!ifile.good())
	{
		ifile.close();
		this->mRequest[fd]->setResource(this->mConfig->getDefaultMap()["error_page"], file);
		ifile.open(this->mRequest[fd]->getResource().c_str());
	}
	while (getline(ifile, tmp))
	{
		fileContent.append(tmp);
		fileContent.append("\r\n");
	}
	this->generateResponse(fd, fileContent.size());
	this->mResponse.append(fileContent);
	// std::cout << this->mResponse << std::endl;
	// möglicherweise gesendete bytes abgleichen mit den zu sendenden und ggf. senden wiederholen
	// flag MSG_DONTWAIT statt 0 und nach EAGAIN/EWOULDBLOCK abfragen
	send(fd, this->mResponse.c_str(), this->mResponse.size(), 0);
	// close(this->mMsgFD);
	this->mRespCode = "200 OK";
}

/**
 * @brief maps errors to error html pages
 * 
 * @param fd 
 * @param error 
 */
void	httpServer::errorHandler(int fd, std::string error)
{
	if (DEBUG > 2)
		std::cout << "httpServer errorHandler" << std::endl;
	int	errorNo = 0;

	errorNo = atoi(error.c_str());
	this->mRespCode = error;
	switch (errorNo)
	{
		case 201:
			this->answer(fd, "201created.html");
			break;
		case 400:
			this->answer(fd, "400bad_request.html");
			break;
		case 401:
			this->answer(fd, "401unauthorized.html");
			break;
		case 402:
			this->answer(fd, "000default.html");
			break;
		case 403:
			this->answer(fd, "403forbidden.html");
			break;
		case 404:
			this->answer(fd, "404not_found.html");
			break;
		case 405:
			this->answer(fd, "405method_not_allowed.html");
			break;
		case 413:
			this->answer(fd, "413req_entity_too_large.html");
			break;
		case 500:
			this->answer(fd, "500internal_server_error.html");
			break;
		case 505:
			this->answer(fd, "505httpvernotsupported.html");
			break;
		
		default:
			this->answer(fd, "000default.html");
	}
}

/**
 * @brief outputs server names and ports to standard output
 * 
 */
void	httpServer::announce(void) const
{
	std::cout << C_GREEN << "Server" << C_GREY << " ( " << this->mConfig->getHost() << ":";
	std::cout << this->mConfig->getPort() << " ) " << this->mConfig->getServerNames();
	std::cout << C_DEF << C_GREEN << " started" << C_DEF << std::endl;
}

int		httpServer::getSocket(void)
{
	return(this->mSocket);
}

char **httpServer::setEnv(int fd, std::string queryString)
{
	if (queryString.size() > 0)
	{
		std::string execution;
		if (queryString == "php")
			execution = "/usr/bin/php-cgi";
		else if (queryString == "py")
			execution = "/usr/bin/python3";
		else if (queryString == "pl")
			execution = "/usr/bin/perl";
		char **args = new char*[3];

		args[0] = strdup(execution.c_str());
		args[1] = strdup(this->mRequest[fd]->getResource().c_str());
		args[2] = NULL;
		return (args);
	}
	else
	{
		char **envp = new char*[5]();
		envp[0] = strdup(("REQUEST_METHOD=" + this->mRequest[fd]->getReqType()).c_str());
		if (this->mRequest[fd]->getReqType() == "POST")
			envp[1] = strdup(("QUERY_STRING=" + this->mRequest[fd]->getPayload()).c_str());
		else
			envp[1] = strdup(("QUERY_STRING=" + this->mRequest[fd]->getQuery()).c_str());
		envp[2] = strdup(("PATH_INFO=" + this->mRequest[fd]->getResource()).c_str());
		envp[3] = strdup(("PATH_TRANSLATED=" + this->mRequest[fd]->getResource()).c_str());
		envp[4] = NULL;
		return (envp);	
	}
	return (NULL);
}

/**
 * @brief enables directory listing on server
 */
void httpServer::handleDirListing(int fd)
{
	std::ifstream 	ifile;
	std::string		tmp;
	std::string		fileContent;

	ifile.open("./www/dirlisting.html");
	while (getline(ifile, tmp))
	{
		fileContent.append(tmp);
		fileContent.append("\r\n");
	}
	ifile.close();
	std::string path = this->mRequest[fd]->getResource();
	DIR *dir;
	struct dirent *ent;
	std::string content;
	if ((dir = opendir (path.c_str())) != NULL) {
	while ((ent = readdir (dir)) != NULL) {
		content = "<tr><td>";
		content.append(ent->d_name);
		content.append("</td></tr>\r\n");
		fileContent.append(content);
	}
	closedir (dir);
	} else {
		throw std::logic_error("404 Not Found");
	}
	fileContent.append("</table></body></html>");

	this->generateResponse(fd, fileContent.size());
	this->mResponse.append(fileContent);
}

/**
 * @brief enables execution of executable files in php or python or perl
 */
std::string httpServer::handleCGI(int fd)
{
	std::string 	tmpFile;
	std::string		tmp;


	srand(time(NULL));
	int	nb = rand();
	pid_t pid = -1;
	tmpFile = "/tmp/" + this->mRequest[fd]->getFileName();
	tmpFile.append(ft_itoa(nb));
	int tempFd 	= open(tmpFile.c_str(), O_RDWR|O_CREAT, 0644);
	pid = fork();
	int status = 0;
	this->mEnv = setEnv(fd, "");
	char **args = setEnv(fd, this->mRequest[fd]->getFileExt()); 
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
			ws_destroy_array(args);
			ws_destroy_array(this->mEnv);
			if (WIFEXITED(status) && WEXITSTATUS(status))
				throw std::logic_error("500 Internal Server Error");
		}
	}
	catch(const std::logic_error& e)
	{
		this->errorHandler(fd, e.what());
	}
	return (tmpFile);
	/* 
	(achtung leaking FDs)
	*/
}
std::map<int, std::string>	httpServer::getMsg(void) const
{
	return (this->mMsg);
}

void	httpServer::eraseMsg(int fd)
{
	if (this->mMsg.erase(fd) != 1)
		throw std::logic_error("Error: Failed to erase FD/Msg");
}

bool	httpServer::generateRequest(int fd)
{
	try
	{
		this->mRequest[fd] = new httpRequest(this->mMsg[fd], *this->mConfig);
	}
	catch(const std::exception& e)
	{
		this->errorHandler(fd, e.what());
		return true;
	}
	return false;
}
