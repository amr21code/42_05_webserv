/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpServer.class.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:51:13 by anruland          #+#    #+#             */
/*   Updated: 2022/10/13 15:41:32 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPSERVER_CLASS_HPP
#define HTTPSERVER_CLASS_HPP
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
#include "httpRequest.class.hpp"
#include "incl.hpp"
#include <sys/epoll.h>
#include <sstream>


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
		void	answer(void);
		void	answer(std::string file);
		void	errorHandler(std::string error);

	private:
		std::string			mServerName; //config
		static const int	mcConfDomain = AF_INET;  //m = member, c = const, Conf = config
		static const int	mcConfComType = SOCK_STREAM;
		static const int	mcConfProtocol = 0;
		static const int	mcConfBufSize = 30;
		int					mSocket;
		// int					mPort;
		struct sockaddr_in	mSockAddr;
		httpConfig			*mConfig;
		httpRequest			*mRequest;

		//new class
		int					mMsgFD;
		std::string			mIncMsg;


		
};
#endif