/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:51:13 by anruland          #+#    #+#             */
/*   Updated: 2022/09/20 14:25:30 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP
#include <sys/socket.h>
#include <string>
#include <iostream>
#ifndef DEBUG
#define DEBUG 0
#endif

class httpServer
{
	public:
		httpServer(void);
		~httpServer(void);
		void	openSocket(void);
		void	closeSocket(void);
		void	log(std::string &message) const;

	private:
		std::string			mServerName; //config
		static const int	mcConfDomain = AF_INET;  //m = member, c = const, Conf = config
		static const int	mcConfComType = SOCK_STREAM;
		static const int	mcConfProtocol = 0;
		int					mSocket;
		int					mPort;
		long				m
};
#endif