/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpServer.class.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: raweber <raweber@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:51:13 by anruland          #+#    #+#             */
/*   Updated: 2022/10/31 14:36:07 by raweber          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPSERVER_CLASS_HPP
#define HTTPSERVER_CLASS_HPP
#include "httpConfig.class.hpp"
#include "httpRequest.class.hpp"
#include "incl.hpp"

class httpServer
{
	public:
		httpServer(httpConfig *config);
		~httpServer(void);
		void	openSocket(void);
		void	closeSocket(void);
		void	listenSocket(void);
		void	announce(void) const;
		int		getMsgFD(void);
		int		getSocket(void);
		void	receive(void);
		void	answer(void);
		void	answer(std::string file);
		void	errorHandler(std::string error);
		void	generateResponse(size_t fileSize);
		std::string IntToString(size_t a);
		char 	**setEnv(std::string queryString);
		void	fileUpload(void);
		void	handleDirListing(void);


	private:
		//config
		static const int	mcConfDomain = AF_INET;  //m = member, c = const, Conf = config
		static const int	mcConfComType = SOCK_STREAM;
		static const int	mcConfProtocol = 0;
		static const int	mcConfBufSize = 100;
		std::string			mServerName; //config
		int					mSocket;
		char 				**mEnv;
		// int					mPort;
		struct sockaddr_in	mSockAddr;
		httpConfig			*mConfig;
		// request
		httpRequest			*mRequest;
		int					mMsgFD;
		std::string			mIncMsg;
		// response
		std::string			mResponse;
		std::string			mRespCode;
};
#endif