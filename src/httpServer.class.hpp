/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpServer.class.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djedasch <djedasch@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:51:13 by anruland          #+#    #+#             */
/*   Updated: 2022/11/03 14:29:06 by djedasch         ###   ########.fr       */
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
		void			generateResponse(int fd, size_t fileSize);
		std::string 	IntToString(size_t a);
		char 			**setEnv(int fd, std::string queryString);
		void			fileUpload(int fd);
		void			handleDirListing(int fd);
		std::string		handleCGI(int fd);
		std::map<int, std::string>	getMsg(void) const;
		void			eraseMsg(int fd);
		bool			generateRequest(int fd);



	private:
		//config
		static const int			mcConfDomain = AF_INET; 
		static const int			mcConfComType = SOCK_STREAM;
		static const int			mcConfProtocol = 0;
		static const int			mcConfBufSize = 2048;
		std::string					mServerName;
		int							mSocket;
		char 						**mEnv;
		struct sockaddr_in			mSockAddr;
		httpConfig					*mConfig;
		// request
		std::map<int, httpRequest *>mRequest;
		std::map<int, std::string>	mMsg;
		// response
		std::string					mResponse;
		std::string					mRespCode;
};
#endif