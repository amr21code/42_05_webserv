/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:51:13 by anruland          #+#    #+#             */
/*   Updated: 2022/09/26 14:33:32 by anruland         ###   ########.fr       */
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
#include "httpConfig.hpp"
#ifndef DEBUG
#define DEBUG 0
#endif

class httpServer
{
	public:
		httpServer(void);
		httpServer(std::string configPath);
		~httpServer(void);
		void	openSocket(void);
		void	closeSocket(int socket);
		void	log(std::string &message) const;
		void	listenSocket(void);

	private:
		std::string			mServerName; //config
		static const int	mcConfDomain = AF_INET;  //m = member, c = const, Conf = config
		static const int	mcConfComType = SOCK_STREAM;
		static const int	mcConfProtocol = 0;
		static const int	mcConfBufSize = 30000;
		int					mSocket;
		int					mPort;
		struct sockaddr_in	mSockAddr;

		//new class
		int					mMsgFD;
		std::string			mIncMsg;


		
};
#endif