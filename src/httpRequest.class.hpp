/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpRequest.class.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 16:49:50 by anruland          #+#    #+#             */
/*   Updated: 2022/10/26 14:51:30 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_CLASS_HPP
#define  HTTPREQUEST_CLASS_HPP
#include "incl.hpp"
#include "httpConfig.class.hpp"


class httpRequest
{
    public:
		httpRequest(void);
		httpRequest(std::string msg, httpConfig config);
		httpRequest(std::string errorFile, httpConfig config, int flag);
		~httpRequest(void);
		
		std::string getResource(void) const;
		std::string getReqType(void) const;
		void		setResource(std::string defFolder, std::string errFile);
		std::string	getPayload(void) const;
		void		firstLineHandler(std::string msg, httpConfig config);
		std::string	getFileExt(void) const;
		std::string getFileName(void) const;

	private:
		std::map<std::string, std::string> mRequest;
		std::string	mReqType;
		std::string	mResource;
		std::string	mFileExt;
		std::string mFileName;
		bool		mDirListing;
		std::string	mHost;
		int			mPort;
		bool		mError;
		std::string mPayload;
		
		// add stuff for POST/DELETE
};

#endif