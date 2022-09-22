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

#include "httpServer.hpp"

httpServer::httpServer(void)
{
	if (DEBUG > 2)
		std::cout << "httpServer default constructor" << std::endl;
	this->mSockAddr.sin_family = this->mcConfDomain;
	this->mSockAddr.sin_port = htons(2000);
	this->mSockAddr.sin_addr.s_addr = INADDR_ANY;
	memset(this->mSockAddr.sin_zero, '\0', sizeof this->mSockAddr.sin_zero);
	this->openSocket();
	this->listenSocket();
}

httpServer::~httpServer(void)
{
    if (DEBUG > 2)
		std::cout << "httpServer destructor" << std::endl;
	this->closeSocket(this->mSocket);
}

void httpServer::openSocket(void)
{
    if (DEBUG > 2)
		std::cout << "httpServer openSocket" << std::endl;
    this->mSocket = socket(this->mcConfDomain, this->mcConfComType, this->mcConfProtocol);
    if (this->mSocket < 0)
    {
        std::cerr << "cannot open socket" << std::endl;
		    return ;
    }
	if (bind(this->mSocket, (struct sockaddr *)&this->mSockAddr, sizeof(this->mSockAddr)))
	{
        std::cerr << "cannot bind socket " << errno << std::endl;
		    return ;
	}
}

void httpServer::closeSocket(int socket)
{
    if (DEBUG > 2)
		std::cout << "httpServer closeSocket" << std::endl;
	close(socket);
}

void httpServer::log(std::string &message) const
{
    if (DEBUG > 2)
		std::cout << "httpServer log" << std::endl;
    std::cout << message << std::endl;
}

void	httpServer::listenSocket(void)
{
	int		addrlen = sizeof(this->mSockAddr);
	char	buffer[this->mcConfBufSize];
	int		recv_return = 1;

	if (listen(this->mSocket, 2))
	{
		std::cerr << "cannot listen" << std::endl;
		return ;
	}
	while (1)
	{
  		bzero(buffer, this->mcConfBufSize);
		this->mMsgFD = accept(this->mSocket, (struct sockaddr *)&this->mSockAddr, (socklen_t *)&addrlen);
		if (this->mMsgFD < 0)
		{
		  std::cerr << "cannot accept " << errno << std::endl;
			return ;
		}
		if ((recv_return = recv(this->mMsgFD, buffer, this->mcConfBufSize, 0)) < 0)
		{
			std::cerr << "receive failed " << errno << std::endl;
			return ;
		}
		if (recv_return > 0)
			this->mIncMsg.append(buffer);
		// write(this->mSocket, "hello from server", sizeof("hello from server"));
		send(this->mMsgFD, "hello from server", sizeof("hello from server"), 0);
		std::cout << this->mIncMsg << std::endl;
		this->closeSocket(this->mMsgFD);
  	}
}