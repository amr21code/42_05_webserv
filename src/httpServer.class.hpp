/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpServer.class.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:51:13 by anruland          #+#    #+#             */
/*   Updated: 2022/11/02 17:03:26 by anruland         ###   ########.fr       */
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
		void			openSocket(void);
		void			closeSocket(void);
		void			listenSocket(void);
		void			announce(void) const;
		int				getSocket(void);
		int				receive(void);
		void			answer(int fd);
		void			answer(int fd, std::string file);
		void			errorHandler(int fd, std::string error);
		void			generateResponse(size_t fileSize);
		std::string 	IntToString(size_t a);
		char 			**setEnv(std::string queryString);
		void			fileUpload(void);
		void			handleDirListing(void);
		std::string		handleCGI(int fd);
		std::map<int, std::string>	getMsg(void);



	private:
		//config
		static const int			mcConfDomain = AF_INET;  //m = member, c = const, Conf = config
		static const int			mcConfComType = SOCK_STREAM;
		static const int			mcConfProtocol = 0;
		static const int			mcConfBufSize = 100;
		std::string					mServerName;
		int							mSocket;
		char 						**mEnv;
		struct sockaddr_in			mSockAddr;
		httpConfig					*mConfig;
		// request
		httpRequest					*mRequest;
		std::map<int, std::string>	mMsg;
		// response
		std::string					mResponse;
		std::string					mRespCode;
};
#endif