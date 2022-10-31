/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpRequest.class.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 16:49:53 by anruland          #+#    #+#             */
/*   Updated: 2022/10/31 09:44:52 by anruland         ###   ########.fr       */
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
	if (DEBUG > 2)
		std::cout << "httpRequest constructor with message" << std::endl;
	std::vector<std::string> tmpLines;
	std::vector<std::string> tmpElements;
	this->mDirListing = false;
	this->mRedirect = false;
	this->mError = false;
	tmpLines = explode(msg, "\r\n");
	// std::cout << "!!!!!!!!!!!!!!!ralf begin" << std::endl;
	// std::cout << msg << std::endl;
	// std::cout << "!!!!!!!!!!!!!!!ralf end" << std::endl;
	std::vector<std::string>::iterator it = tmpLines.begin();
	try
	{
		firstLineHandler(*(it), config);
		it++;
		for (; it != tmpLines.end(); it++)
		{
			if (!(*it).compare(0, 29, "-----------------------------"))
				break;
			tmpElements = explodeOneLine(*it, ": ");
			// std::cout << "tmpElements[0] " << tmpElements[0] << "; tmpElements[1] " << tmpElements[1] << std::endl;
			this->mRequest[tmpElements[0]] = tmpElements[1];
		}
		// std::vector<std::string>::iterator itt = tmpLines.begin();
		// for (; itt!= tmpLines.end(); itt++)
		// {
		// 	std::cout << "0" << *itt << std::endl;
		// }
		// std::map<std::string, std::string>::iterator ite = this->mRequest.begin();
		// for (; ite != this->mRequest.end(); ite++)
		// {
		// 	std::cout << ite->first << "-" << ite->second << std::endl;
		// }
	}
	catch(const std::logic_error& e)
	{
		throw e;
	}
	this->mFileExt = this->mResource.substr(this->mResource.find_last_of('.') + 1);
	std::transform(this->mFileExt.begin(), this->mFileExt.end(), this->mFileExt.begin(), ::tolower);
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

void httpRequest::firstLineHandler(std::string msg, httpConfig config)
{
	std::string	tempResourceDir = "/";
	std::string	tempResourceFile;
	size_t		resLength = 0;
	size_t		i = 0;

	this->mLocNb = -1;
	if (msg.size() > config.getMaxBodySize())
		throw std::logic_error("413 Request Entity Too Large");
	else if (msg.size() < 14)
		throw std::logic_error("400 Bad Request");
	if (msg.find("HTTP/1.1") == msg.npos)
		throw std::logic_error("505 HTTP Version Not Supported");
	this->mReqType = msg.substr(0, msg.find(" "));
	this->mResource = msg.substr(msg.find(" ") + 1, msg.find(" ", msg.find(" ") + 1) - msg.find(" ") - 1);
	if (this->mResource.find('?') != std::string::npos)
	{
		this->mQuery = this->mResource.substr(this->mResource.find('?') + 1);
		this->mResource.erase(this->mResource.find('?'));
		// std::cout << "remaining resource " << this->mResource << std::endl;
		// std::cout << "query " << this->mQuery << std::endl;
	}
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
		// if ((*itvec)["location"].size() > resLength && !(*itvec)["location"].compare(0, (*itvec)["location"].size(), tempResourceDir))
		if ((*itvec)["location"].size() > resLength && tempResourceDir.substr(0, (*itvec)["location"].size()) == (*itvec)["location"])
		{
			resLength = (*itvec)["location"].size();
			this->mLocNb = i;
		}
		i++;
	}
	// std::cout << "this->mLocNb " << this->mLocNb << std::endl;
	// std::cout << "reslen " << resLength << std::endl;
	if (this->mLocNb == static_cast<long unsigned int>(-1))
		throw std::logic_error("404 Not Found");
	if (config.getConfLocations()[this->mLocNb]["allowed_methods"].find(this->mReqType) == std::string::npos)
		throw std::logic_error("405 Method Not Allowed");
	this->mResource = config.getConfLocations()[this->mLocNb]["root"];
	if (config.getConfLocations()[this->mLocNb]["redirect"].size() > 0)
	{
		this->mResource = config.getConfLocations()[this->mLocNb]["redirect"];
		this->mRedirect = true;
		return ;
	}
	tempResourceDir.erase(0, resLength);
	this->mResource.append(tempResourceDir);
	if (tempResourceFile.size())
	{
		this->mResource.append(tempResourceFile);
		this->mFileName = tempResourceFile;
	}
	else if (!this->mReqType.compare("GET"))
	{
		if (config.getConfLocations()[this->mLocNb]["autoindex"].compare("off"))
		{
			std::vector<std::string> tmp = explode(config.getConfLocations()[this->mLocNb]["index"], ",");
			std::string	tmpResource;
			for (size_t j = 0; j < tmp.size(); j++)
			{
				tmpResource = this->mResource;
				tmpResource.append(tmp[j]);
				this->mFileName = tmp[j];
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
		if (this->mResource[this->mResource.size() - 1] == '/' && !config.getConfLocations()[this->mLocNb]["dirlisting"].compare("on"))
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
}

std::string	httpRequest::getFileExt(void) const
{
	return (this->mFileExt);
}

std::string	httpRequest::getFileName(void) const
{
	return (this->mFileName);
}
std::string	httpRequest::getQuery(void) const
{
	return (this->mQuery);
}

bool		httpRequest::getDirListing(void) const
{
	return (this->mDirListing);
}

bool		httpRequest::getRedirect(void) const
{
	return (this->mRedirect);
}

size_t		httpRequest::getLocNb(void) const
{
	return (this->mLocNb);
}