/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpRequest.class.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 16:49:53 by anruland          #+#    #+#             */
/*   Updated: 2022/10/17 13:48:31 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpRequest.class.hpp"

httpRequest::httpRequest(void)
{
	if (DEBUG > 2)
		std::cout << "httpConfig default constructor" << std::endl; 
}
httpRequest::httpRequest(std::string msg)
{
	if (DEBUG > 2)
		std::cout << "httpConfig constructor with message" << std::endl;
	std::cout << msg << std::endl;
	std::cout << msg.find("HTTP/1.1") << " >= " << msg.npos << std::endl;
	if (msg.find("HTTP/1.1") == msg.npos)
		throw std::logic_error("403");
	this->mReqType = msg.substr(0, msg.find(" "));
	this->mResource = msg.substr(msg.find(" ") + 1, msg.find(" ", msg.find(" ") + 1) - msg.find(" "));
//	
//RESOURCE PARSING -> find requested file based on rules/config/routes
//
	std::cout << this->mReqType << " " << this->mResource << std::endl;
	// std::stringstream ss;
	// ss << msg;
	// std::string token;
	// while(std::getline(ss, token, ' ')) {
	// 	std::cout << token << '\n';
	// }
}

httpRequest::~httpRequest(void)
{
	if (DEBUG > 2)
        std::cout << "httpConfig destructor" << std::endl;
}

std::string httpRequest::getResource(void) {

	return (this->mResource);
}