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

int	httpServer::receive(void)
{
    if (DEBUG > 2)
		std::cout << "httpServer receive" << std::endl;

	int					addrlen = sizeof(this->mSockAddr);
	std::vector<char>	buffer(mcConfBufSize + 1, '\0');
	int					recv_return = 1;
	int					tmpfd = -1;
	std::string			tmpmsg;

	tmpfd = accept(this->mSocket, (struct sockaddr *)&this->mSockAddr, (socklen_t *)&addrlen);
	if (tmpfd < 0)
	{
		std::cerr << "Error: accept() failed" << std::endl;
		return (-1);
	}
	tmpmsg = "";
	while (tmpmsg.size() == 0)
	{
		while ((recv_return = recv(tmpfd, buffer.data(), this->mcConfBufSize, MSG_DONTWAIT)) > 0)
		{
			if (DEBUG > 2)
				std::cout << "httpServer received something" << std::endl;
			if (tmpmsg.size() == 0)
				tmpmsg = buffer.data();
			else
				tmpmsg.append(buffer.data(), recv_return); 
			buffer.assign(this->mcConfBufSize + 1, '\0');
			usleep(100);
		}
	}
	this->mMsg[tmpfd] = tmpmsg;
	return (tmpfd);
}

void	httpServer::fileUpload(void)
{
	std::map<std::string, std::string>	tmpRequest = this->mRequest.getRequest();
	size_t		 						length = atoi(tmpRequest["Content-Length"].c_str());
	std::string 						boundary = tmpRequest["Content-Type"].substr(tmpRequest["Content-Type"].find("=")+1);
	std::string							tmpPayload = this->mRequest.getPayload();
	size_t								payloadPos = 0;
	int									nbChunks = -1;
	size_t								lineEnd = 0;
	std::string							fileName;
	std::string							content;
	std::fstream						file;
	std::string							path;
	
	// std::cout << "bound " << boundary << std::endl;
	// std::cout << "pay " << tmpPayload << std::endl;

	while (payloadPos != std::string::npos)
	{
		payloadPos = tmpPayload.find(boundary, payloadPos + boundary.size());
		nbChunks++;
	}

	// std::cout << "Chunks " << nbChunks << std::endl;
	payloadPos = tmpPayload.find(boundary);
	while (nbChunks >= 0)
	{
		/*
		finde erste boundary -> suchindex hinter boundary
		finde content disposition an pos 0 und finde filename="
		speichere filename ab (optional bis dahin kein \r\n)
		suchindex hinter \r\n\r\n
		finde nächste boundary und substr von suchindex bis boundary - 1
		wenn letzte boundary nicht auf -- endet -> error
		*/
		// std::cout << "find bound true" << std::endl;
		payloadPos = payloadPos + boundary.size() + 2;
		if (tmpPayload.find("Content-Disposition") == payloadPos)
		{
			lineEnd = tmpPayload.find("\r\n", payloadPos);
			// std::cout << "ppos " << tmpPayload.find("filename=", payloadPos) << std::endl;
			if ((payloadPos = tmpPayload.find("filename=", payloadPos)) < lineEnd)
			{
				fileName = tmpPayload.substr(payloadPos + 10, tmpPayload.find("\"", payloadPos + 10) - payloadPos - 10);

				// std::cout << "filename " << fileName << std::endl;
				// std::cout << "payload pos " << payloadPos << std::endl;
				// std::cout << "line end " << lineEnd << std::endl;
				std::cout << this->mConfig->getConfLocations()[this->mRequest.getLocNb()]["upload"] << std::endl;
				path = ((path.append(this->mConfig->getConfLocations()[this->mRequest.getLocNb()]["root"])).append(this->mConfig->getConfLocations()[this->mRequest.getLocNb()]["upload"])).append(fileName);
				file.open(path.c_str(), std::fstream::out | std::fstream::trunc);
				if (!file.good())
					throw std::logic_error("404 Not Found");
				payloadPos = tmpPayload.find("\r\n\r\n") + 4;
				content = tmpPayload.substr(payloadPos, tmpPayload.find(boundary, payloadPos) - payloadPos - 2);
				// std::cout << "RALF" << content << "RALF" << std::endl;
				file.write(&content[0], content.size());
				file.close();
			}
			else
				payloadPos = tmpPayload.find("\r\n\r\n") + 4;
		}
		// else if (tmpPayload.find("--") == payloadPos)
		// {
		// 	// std::cout << "transmission end" << std::endl;
		// 	break ;
		// }
		payloadPos = tmpPayload.find(boundary, payloadPos);
		// std::cout << "payload " << std::endl;
		nbChunks--;
		// std::cout << "contlen " << length << std::endl;
		// std::cout << "lenpayl " << tmpPayload.size() << std::endl;
		// std::cout << "maxlen " << tmpPayload.max_size() << std::endl;
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

void	httpServer::generateResponse(size_t fileSize)
{
	if (DEBUG > 2)
		std::cout << "httpServer generateResponse" << std::endl;

	std::vector<char>	buf(this->mcConfBufSize + 1, '\0');
	time_t				now = time(0);
	struct tm			tm = *gmtime(&now);
	std::ifstream 		ifile;
	struct stat			fileStats;
	timespec			modTime;

	if (stat(this->mRequest.getResource().c_str(), &fileStats) == 0)
		modTime = fileStats.st_mtim;
	ifile.open(this->mRequest.getResource().c_str());
	strftime(buf.data(), this->mcConfBufSize, "%a, %d %b %Y %H:%M:%S %Z", &tm);
	this->mResponse = "HTTP/1.1 ";
	this->mResponse.append(this->mRespCode);
	if (this->mRequest.getRedirect())
	{
		this->mResponse.append("\r\nLocation: ");
		this->mResponse.append(this->mRequest.getResource());
	}
	this->mResponse.append("\r\nDate: ");
	this->mResponse.append(buf.data());
	this->mResponse.append("\r\nServer: WebSurfer/0.1.2 (Linux)");
	if (fileSize > 0)
	{
		if (!this->mRequest.getDirListing())
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
			this->mRequest = httpRequest(this->mMsg[fd], *this->mConfig);
			if (this->mRequest.getReqType() == "POST" && !this->mRequest.getRequest()["Content-Type"].find(" multipart/form-data; boundary="))
				this->fileUpload();
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
	if (this->mRequest.getRedirect())
	{
		this->mRespCode = "301 Moved Permanently";
		this->generateResponse(0);
	}
	else if (this->mRequest.getDirListing() && this->mRequest.getReqType() == "GET")
	{
		try 
		{
			handleDirListing();
		}
		catch(const std::logic_error &e)
		{
			this->errorHandler(fd, e.what());
			return ;
		}
	}
	else if (this->mRequest.getReqType() == "GET" || this->mRequest.getReqType() == "POST")
	{
		ifile.open(this->mRequest.getResource().c_str());
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
		if (!this->mRequest.getFileExt().compare("php") || !this->mRequest.getFileExt().compare("py") || !this->mRequest.getFileExt().compare("pl"))
		{
			tmpFile = handleCGI(fd);
			ifile.close();
			ifile.open(tmpFile.c_str());
			if (!this->mRequest.getFileExt().compare("php"))
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
		this->generateResponse(fileContent.size());
		this->mResponse.append(fileContent);
	}
	else if (this->mRequest.getReqType() == "DELETE")
	{
		ifile.open(this->mRequest.getResource().c_str());
		try
		{
			if (!ifile.good())
				throw std::logic_error("404 Not Found");
			ifile.close();
			remove(this->mRequest.getResource().c_str());
		}
		catch(const std::logic_error& e)
		{
			this->errorHandler(fd, e.what());
			return ;
		}
		this->generateResponse(fileContent.size());
		this->mResponse.append(fileContent);
	}
	else
		throw std::logic_error("405 Method Not Allowed");
	// std::cout << "before send" << std::endl;
	send(fd, this->mResponse.c_str(), this->mResponse.size(), 0);
	// std::cout << "after send" << std::endl;
	// if send fails -> remove msgfd from epoll
	// delete this->mRequest;
}

void	httpServer::answer(int fd, std::string file)
{
	if (DEBUG > 2)
		std::cout << "httpServer answer with file" << std::endl;
	std::ifstream 	ifile;
	std::string		tmp;
	std::string		fileContent;
	this->mRequest = httpRequest(file, *this->mConfig, 1);
	ifile.open(this->mRequest.getResource().c_str());
	if (!ifile.good())
	{
		ifile.close();
		this->mRequest.setResource(this->mConfig->getDefaultMap()["error_page"], file);
		ifile.open(this->mRequest.getResource().c_str());
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
	send(fd, this->mResponse.c_str(), this->mResponse.size(), 0);
	// close(this->mMsgFD);
	this->mRespCode = "200 OK";
}

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

char **httpServer::setEnv(std::string queryString)
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
		// args[1] = strdup("/home/pi/projects/C05_webserv/42_05_webserv/www/html/phptest/test.php");
		args[1] = strdup(this->mRequest.getResource().c_str());
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
		envp[0] = strdup(("REQUEST_METHOD=" + this->mRequest.getReqType()).c_str());
		if (this->mRequest.getReqType() == "POST")
			envp[1] = strdup(("QUERY_STRING=" + this->mRequest.getPayload()).c_str());
		else
			envp[1] = strdup(("QUERY_STRING=" + this->mRequest.getQuery()).c_str());
		envp[2] = strdup(("PATH_INFO=" + this->mRequest.getResource()).c_str());
		envp[3] = strdup(("PATH_TRANSLATED=" + this->mRequest.getResource()).c_str());
		// envp[3] = strdup("CONTENT_LENGTH=7");
		envp[4] = NULL;
		return (envp);	
	}
	//explode querystring '&'
	return (NULL);
}

void httpServer::handleDirListing(void)
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
	std::string path = this->mRequest.getResource();
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
		/* could not open directory */
		throw std::logic_error("404 Not Found");
	}
	fileContent.append("</table></body></html>");

	this->generateResponse(fileContent.size());
	this->mResponse.append(fileContent);
}

std::string httpServer::handleCGI(int fd)
{
	std::string 	tmpFile;
	std::string		tmp;


	srand(time(NULL));
	int	nb = rand();
	pid_t pid = -1;
	tmpFile = "/tmp/" + this->mRequest.getFileName();
	tmpFile.append(ft_itoa(nb));
	// ifile.close();
	int tempFd 	= open(tmpFile.c_str(), O_RDWR|O_CREAT, 0644);
	pid = fork();
	int status = 0;
	this->mEnv = setEnv("");
	char **args = setEnv(this->mRequest.getFileExt()); // this->mRequest.getQuery()
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
