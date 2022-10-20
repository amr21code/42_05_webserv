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
		throw std::logic_error("Error: cannot bind socket");
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
	// int		addrlen = sizeof(this->mSockAddr);
	// char	buffer[this->mcConfBufSize];
	// int		recv_return = 1;

	if (listen(this->mSocket, 2))
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
	int		addrlen = sizeof(this->mSockAddr);
	std::vector<char> buffer(mcConfBufSize + 1, '\0');
	//char* buffer = new char[this->mcConfBufSize + 1];
	//char	buffer[this->mcConfBufSize + 1];
	int		recv_return = 1;

	this->mMsgFD = accept(this->mSocket, (struct sockaddr *)&this->mSockAddr, (socklen_t *)&addrlen);
	if (this->mMsgFD < 0)
	{
		std::cerr << "Error: accept() failed" << std::endl;
		return ;
	}
  	// bzero(buffer, this->mcConfBufSize);
	// if ((recv_return = recv(this->mMsgFD, buffer, this->mcConfBufSize, 0)) < 0)
	// {
	// 	std::cerr << "Error: receive() failed" << std::endl;
	// 	return ;
	// }
	this->mIncMsg = "";
  	//bzero(buffer, this->mcConfBufSize);
	buffer[0] = 42;
	// int i = 0;
	while ((recv_return = recv(this->mMsgFD, buffer.data(), this->mcConfBufSize, MSG_DONTWAIT)) > 0)
	{
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
	}
	std::cout << this->mIncMsg << std::endl;
	if (this->mIncMsg.size() > 0)
	{
		try
		{
			// std::cout << "try" << std::endl;
			this->mRequest = new httpRequest(this->mIncMsg, *this->mConfig);
		}
		catch(const std::exception& e)
		{
			// std::cerr << e.what() << '\n';
			this->errorHandler(e.what());
		}
		this->answer();
	}
	// std::cout << "TEST" << std::endl;
}

std::string httpServer::IntToString(size_t a)
{
    std::ostringstream temp;
    temp << a;
    return temp.str();
}

void	httpServer::generateResponse(size_t fileSize)
{
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
	this->mResponse.append("\r\nServer: WebSurfer/0.1.2 (Linux)\r\nLast-Modified: ");
	bzero(buf, 100);
	strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", gmtime(&modTime.tv_sec));
	this->mResponse.append(buf);
	this->mResponse.append("\r\nContent-Length: ");
	this->mResponse.append(this->IntToString(fileSize));
	this->mResponse.append("\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n");
}

void	httpServer::answer(void)
{
	std::ifstream 	ifile;
	std::string		tmp;
	std::string		fileContent;
	// std::cout << this->mRequest->getResource() << std::endl;
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
	}
	while (getline(ifile, tmp))
	{
		fileContent.append(tmp);
		fileContent.append("\r\n");
	}
	this->generateResponse(fileContent.size());
	this->mResponse.append(fileContent);
	// std::cout << this->mResponse << std::endl;
	// std::string msg = "HTTP/1.1 200 OK";
	// msg.append(this->mConfig->getServerNames());
	// std::cout << msg.c_str() << std::endl;
	send(this->mMsgFD, this->mResponse.c_str(), this->mResponse.size(), 0);
	// std::cout << this->mIncMsg << std::endl;
	ifile.close();
	close(this->mMsgFD);
}

void	httpServer::answer(std::string file)
{
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
	send(this->mMsgFD, this->mResponse.c_str(), this->mResponse.size(), 0);
	close(this->mMsgFD);
}

void	httpServer::errorHandler(std::string error)
{
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