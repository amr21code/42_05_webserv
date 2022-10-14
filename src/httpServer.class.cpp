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
	char	buffer[this->mcConfBufSize];
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
	this->mIncMsg.erase();
  	bzero(buffer, this->mcConfBufSize);
	while ((recv_return = recv(this->mMsgFD, buffer, this->mcConfBufSize, MSG_DONTWAIT)) > 0)
	{
		this->mIncMsg.append(buffer);
  		bzero(buffer, this->mcConfBufSize);
	}
	std::cout << "recv " << recv_return << std::endl;
	if (this->mIncMsg.size() > 0)
	{
		try
		{
			// std::cout << this->mIncMsg << std::endl;
			std::cout << "try" << std::endl;
			this->mRequest = new httpRequest(this->mIncMsg);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			this->errorHandler(e.what());
		}
		this->answer();
	}
	// std::cout << "TEST" << std::endl;
}

void	httpServer::answer(void)
{
	std::ifstream 	ifile;
	std::string		tmp;
	std::string		msg;
	ifile.open("test.html");
	while (getline(ifile, tmp))
	{
		msg.append(tmp);
		msg.append("\n");
	}
	// std::string msg = "HTTP/1.1 200 OK";
	// msg.append(this->mConfig->getServerNames());
	// std::cout << msg.c_str() << std::endl;
	send(this->mMsgFD, msg.c_str(), msg.size(), 0);
	// std::cout << this->mIncMsg << std::endl;
	close(this->mMsgFD);
}

void	httpServer::answer(std::string file)
{
	std::ifstream 	ifile;
	std::string		tmp;
	std::string		msg;
	ifile.open(file.c_str());
	while (getline(ifile, tmp))
	{
		msg.append(tmp);
		msg.append("\n");
	}
	// std::string msg = "HTTP/1.1 200 OK";
	// msg.append(this->mConfig->getServerNames());
	// std::cout << msg.c_str() << std::endl;
	send(this->mMsgFD, msg.c_str(), msg.size(), 0);
	// std::cout << this->mIncMsg << std::endl;
	close(this->mMsgFD);
}

void	httpServer::errorHandler(std::string error)
{
	int	errorNo = 0;

	errorNo = atoi(error.c_str());
	switch (errorNo)
	{
		case 400:
			this->answer("./www/errors/400bad_request.html");
			break;

		case 401:
			this->answer("./www/errors/401unauthorized.html");
			break;

		case 402:
			break;

		case 403:
			this->answer("./www/errors/403forbidden.html");
			break;

		case 404:
			this->answer("./www/errors/404not_found.html");
			break;
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