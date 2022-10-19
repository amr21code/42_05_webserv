/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpRequest.class.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 16:49:53 by anruland          #+#    #+#             */
/*   Updated: 2022/10/19 14:06:10 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpRequest.class.hpp"

httpRequest::httpRequest(void)
{
	if (DEBUG > 2)
		std::cout << "httpConfig default constructor" << std::endl; 
}
httpRequest::httpRequest(std::string msg, httpConfig config)
{
	std::string	tempResourceDir = "/";
	std::string	tempResourceFile;
	if (DEBUG > 2)
		std::cout << "httpConfig constructor with message" << std::endl;
	std::cout << msg << std::endl;
	std::cout << msg.find("HTTP/1.1") << " >= " << msg.npos << std::endl;
	if (msg.find("HTTP/1.1") == msg.npos)
		throw std::logic_error("403");
	this->mReqType = msg.substr(0, msg.find(" "));
	this->mResource = msg.substr(msg.find(" ") + 1, msg.find(" ", msg.find(" ") + 1) - msg.find(" ") - 1);
	if (this->mResource[this->mResource.size() - 1] != '/')
	{
		if (this->mResource.find_last_of(".") == this->mResource.npos)
				this->mResource.append("/");
		tempResourceFile = this->mResource.substr(this->mResource.find_last_of('/') + 1);
		std::cout << "Temp Resource Dir is: " << tempResourceDir << std::endl;
		std::cout << "Temp Resource File is: " << tempResourceFile << std::endl;
	}
	// APPEND '/' if no /  at the end of the path
	tempResourceDir = this->mResource.substr(0, this->mResource.find_last_of('/') + 1);
	std::vector<std::map<std::string, std::string> >::iterator itvec = config.getConfLocations().begin();
	for (; itvec != config.getConfLocations().end(); itvec++)
	{
		if (!(*itvec)["location"].compare(tempResourceDir.c_str()))
			std::cout << "location successful " << tempResourceDir << std::endl;
		else
			std::cout << "location unsuccessful " << tempResourceDir << std::endl;
		// tempResourceFile = this->mResource.substr(this->mResource.find_last_of('/') + 1); FIND INDEX
	}


//	
//RESOURCE PARSING -> find requested file based on rules/config/routes
//
	// std::cout << this->mReqType << " -" << this->mResource << "-" << std::endl;
}

httpRequest::~httpRequest(void)
{
	if (DEBUG > 2)
        std::cout << "httpConfig destructor" << std::endl;
}

std::string httpRequest::getResource(void) {

	return (this->mResource);
}