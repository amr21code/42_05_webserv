/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpServer.class.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:51:13 by anruland          #+#    #+#             */
/*   Updated: 2022/10/12 13:55:58 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cerrno> //entfernen
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include "httpConfig.class.hpp"
#include "incl.hpp"
#include <sys/epoll.h>
#ifndef DEBUG
#define DEBUG 0
#endif

class httpServer
{
	public:
		httpServer(void);
		httpServer(std::string configPath);
		httpServer(httpConfig *config);
		~httpServer(void);
		void	openSocket(void);
		void	closeSocket(void);
		void	log(std::string &message) const;
		void	listenSocket(void);
		void	announce(void) const;
		int		getMsgFD(void);
		int		getSocket(void);
		void	receive(void);
		void	answer(struct epoll_event epevent);

	private:
		std::string			mServerName; //config
		static const int	mcConfDomain = AF_INET;  //m = member, c = const, Conf = config
		static const int	mcConfComType = SOCK_STREAM;
		static const int	mcConfProtocol = 0;
		static const int	mcConfBufSize = 30000;
		int					mSocket;
		// int					mPort;
		struct sockaddr_in	mSockAddr;
		httpConfig			*mConfig;

		//new class
		int					mMsgFD;
		std::string			mIncMsg;


		
};
#endif