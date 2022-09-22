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
	mSockAddr.sin_family = this->mcConfDomain;
	mSockAddr.sin_port = htons(81);
	mSockAddr.sin_addr.s_addr = INADDR_ANY;
	this->openSocket();
	this->listenSocket();
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
    {
        std::cerr << "cannot open socket" << std::endl;
		    return ;
    }
	std::cout << this->mSockAddr.sin_family << " " << this->mSockAddr.sin_port << std::endl;
	if (bind(this->mSocket, (struct sockaddr *)&this->mSockAddr, sizeof(this->mSockAddr)))
	{
        std::cerr << "cannot bind socket " << errno << std::endl;
		    return ;
	}
}

void httpServer::closeSocket(void)
{
    if (DEBUG > 2)
		std::cout << "httpServer closeSocket" << std::endl;
}

void httpServer::log(std::string &message) const
{
    if (DEBUG > 2)
		std::cout << "httpServer log" << std::endl;
    std::cout << message << std::endl;
}

void	httpServer::listenSocket(void)
{
	if (listen(this->mSocket, 2))
	{
		std::cerr << "cannot listen" << std::endl;
		return ;
	}
	while (1)
	{
		this->mMsgFD = accept(this->mSocket, (struct sockaddr *)&this->mSockAddr, (socklen_t *)sizeof(this->mSockAddr));
		if (this->mMsgFD < 0)
		{
		  std::cerr << "cannot accept" << std::endl;
			return ;
		}
  }
}