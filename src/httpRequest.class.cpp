/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpRequest.class.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 16:49:53 by anruland          #+#    #+#             */
/*   Updated: 2022/10/24 13:38:47 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpRequest.class.hpp"

httpRequest::httpRequest(void)
{
	if (DEBUG > 2)
		std::cout << "httpRequest default constructor" << std::endl; 
}

httpRequest::httpRequest(std::string msg, httpConfig config)
{
	std::string	tempResourceDir = "/";
	std::string	tempResourceFile;
	size_t		resLength = 0;
	size_t		locNb = -1;
	size_t		i = 0;
	this->mDirListing = false;
	this->mError = false;
	if (DEBUG > 2)
		std::cout << "httpRequest constructor with message" << std::endl;
	if (msg.size() > config.getMaxBodySize())
		throw std::logic_error("413 Request Entity Too Large");
	if (msg.find("HTTP/1.1") == msg.npos)
		throw std::logic_error("505 HTTP Version Not Supported");
	this->mReqType = msg.substr(0, msg.find(" "));
	this->mResource = msg.substr(msg.find(" ") + 1, msg.find(" ", msg.find(" ") + 1) - msg.find(" ") - 1);
	if (this->mResource[this->mResource.size() - 1] != '/')
	{
		if (this->mResource.find_last_of(".") == this->mResource.npos)
				this->mResource.append("/");
		tempResourceFile = this->mResource.substr(this->mResource.find_last_of('/') + 1);
		// std::cout << "Temp Resource Dir is: " << tempResourceDir << std::endl;
		// std::cout << "Temp Resource File is: " << tempResourceFile << std::endl;
	}
	tempResourceDir = this->mResource.substr(0, this->mResource.find_last_of('/') + 1);
	std::vector<std::map<std::string, std::string> >::iterator itvec = config.getConfLocations().begin();
	for (; itvec != config.getConfLocations().end(); itvec++)
	{
		if ((*itvec)["location"].size() > resLength && !(*itvec)["location"].compare(0, (*itvec)["location"].size(), tempResourceDir.c_str()))
		{
			resLength = (*itvec)["location"].size();
			locNb = i;
		}
		i++;
	}
	if (config.getConfLocations()[locNb]["allowed_methods"].find(this->mReqType) == std::string::npos)
		throw std::logic_error("405 Method Not Allowed");
	this->mResource = config.getConfLocations()[locNb]["root"];
	tempResourceDir.erase(0, resLength);
	this->mResource.append(tempResourceDir);
	if (tempResourceFile.size())
		this->mResource.append(tempResourceFile);
	else if (!this->mReqType.compare("GET"))
	{
		if (config.getConfLocations()[locNb]["autoindex"].compare("off"))
		{
			std::vector<std::string> tmp = explode(config.getConfLocations()[locNb]["index"], ',');
			std::string	tmpResource;
			for (size_t j = 0; j < tmp.size(); j++)
			{
				tmpResource = this->mResource;
				tmpResource.append(tmp[j]);
				std::ifstream test(tmpResource.c_str());
    			if (test.good())
				{
					this->mResource = tmpResource;
					test.close();
					break;
				}
				test.close();
			}
		}
		if (this->mResource[this->mResource.size() - 1] == '/' && !config.getConfLocations()[locNb]["dirlisting"].compare("on"))
		{
			this->mDirListing = true;
		}
		else if (this->mResource[this->mResource.size() - 1] == '/')
		{
			throw std::logic_error("404 Not Found");
		}
	}
	else
		throw std::logic_error("403 Forbidden");
	int payloadindex = msg.find("\r\n\r\n", 0);
	this->mPayload = msg.substr(payloadindex + 4);
}

httpRequest::httpRequest(std::string errorFile, httpConfig config, int flag)
{
	(void) flag;
	if (DEBUG > 2)
		std::cout << "httpRequest constructor for errors" << std::endl;
	this->mError = true;
	this->mResource = config.getConfigMap()["error_page"];
	this->mResource.append(errorFile);
}

httpRequest::~httpRequest(void)
{
	if (DEBUG > 2)
        std::cout << "httpRequest destructor" << std::endl;
}

std::string httpRequest::getResource(void) const {

	return (this->mResource);
}

std::string httpRequest::getReqType(void) const {

	return (this->mReqType);
}

std::string httpRequest::getPayload(void) const {

	return (this->mPayload);
}

void httpRequest::setResource(std::string defFolder, std::string errFile) {
	
	this->mResource = defFolder;
	this->mResource.append(errFile);
}
