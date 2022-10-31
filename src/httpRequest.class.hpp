/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpRequest.class.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: raweber <raweber@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 16:49:50 by anruland          #+#    #+#             */
/*   Updated: 2022/10/31 14:38:00 by raweber          ###   ########.fr       */
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
		std::string getQuery(void) const;
		bool		getDirListing(void) const;
		bool		getRedirect(void) const;
		size_t		getLocNb(void) const;
		std::map<std::string, std::string> getRequest() const;

	private:
		std::map<std::string, std::string> mRequest;
		std::string	mReqType;
		std::string	mResource;
		std::string	mQuery;
		std::string mFileName;
		std::string	mFileExt;
		std::string mPayload;
		std::string	mHost;
		int			mPort;
		bool		mDirListing;
		bool		mRedirect;
		bool		mError;
		size_t		mLocNb;
		
		// add stuff for POST/DELETE
};

#endif