/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpRequest.class.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 16:49:50 by anruland          #+#    #+#             */
/*   Updated: 2022/10/14 15:56:04 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_CLASS_HPP
#define  HTTPREQUEST_CLASS_HPP
#include <string>
#include "incl.hpp"
#include <iostream>
#include <sstream>

class httpRequest
{
    public:
		httpRequest(void);
		httpRequest(std::string msg);
		~httpRequest(void);

	private:
		std::string	mReqType;
		std::string	mResource;
		std::string	mHost;
		int			mPort;
		std::string mResponse;
		// add stuff for POST/DELETE
};

#endif