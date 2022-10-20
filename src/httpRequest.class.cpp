/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpRequest.class.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 16:49:53 by anruland          #+#    #+#             */
/*   Updated: 2022/10/20 17:04:51 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpRequest.class.hpp"

httpRequest::httpRequest(void)
{
	if (DEBUG > 2)
		std::cout << "httpConfig default constructor" << std::endl; 
}

httpRequest::httpRequest(std::string errorFile, httpConfig config)
{
	std::string	tempResourceDir = "/";
	std::string	tempResourceFile;
	size_t		resLength = 0;
	size_t		locNb = -1;
	size_t		i = 0;
	this->mDirListing = false;
	if (DEBUG > 2)
		std::cout << "httpConfig constructor with message" << std::endl;
	// std::cout << errorFile << std::endl;
	// std::cout << errorFile.find("HTTP/1.1") << " >= " << errorFile.npos << std::endl;
	if (errorFile.find("HTTP/1.1") == errorFile.npos)
		throw std::logic_error("505 HTTP Version Not Supported");
	this->mReqType = errorFile.substr(0, errorFile.find(" "));
	this->mResource = errorFile.substr(errorFile.find(" ") + 1, errorFile.find(" ", errorFile.find(" ") + 1) - errorFile.find(" ") - 1);
	if (this->mResource[this->mResource.size() - 1] != '/')
	{
		if (this->mResource.find_last_of(".") == this->mResource.npos)
				this->mResource.append("/");
		tempResourceFile = this->mResource.substr(this->mResource.find_last_of('/') + 1);
		// std::cout << "Temp Resource Dir is: " << tempResourceDir << std::endl;
		// std::cout << "Temp Resource File is: " << tempResourceFile << std::endl;
	}
	// APPEND '/' if no /  at the end of the path
	tempResourceDir = this->mResource.substr(0, this->mResource.find_last_of('/') + 1);
	std::vector<std::map<std::string, std::string> >::iterator itvec = config.getConfLocations().begin();
	for (; itvec != config.getConfLocations().end(); itvec++)
	{
		if ((*itvec)["location"].size() > resLength && !(*itvec)["location"].compare(0, (*itvec)["location"].size(), tempResourceDir.c_str()))
		{
			resLength = (*itvec)["location"].size();
			locNb = i;
			// do stuff
		}
		// 	std::cout << "location successful " << tempResourceDir << std::endl;
		// else
		// 	std::cout << "location unsuccessful " << tempResourceDir << std::endl;
		// tempResourceFile = this->mResource.substr(this->mResource.find_last_of('/') + 1); FIND INDEX
		i++;
	}
	if (config.getConfLocations()[locNb]["allowed_methods"].find(this->mReqType) == std::string::npos)
		throw std::logic_error("405 Method Not Allowed");
	this->mResource = config.getConfLocations()[locNb]["root"];
	tempResourceDir.erase(0, locNb);
	this->mResource.append(tempResourceDir);
	if (tempResourceFile.size())
		this->mResource.append(tempResourceFile);
	else
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
	// if not found -> invalid request
	// std::cout << "i " << locNb << std::endl;
	// std::cout << "len " << resLength << std::endl;
	// std::cout << config.getConfLocations()[locNb]["location"] << std::endl;


//	
//RESOURCE PARSING -> find requested file based on rules/config/routes
//
	// std::cout << this->mReqType << " -" << this->mResource << "-" << std::endl;
}

httpRequest::httpRequest(std::string errorFile, httpConfig config, int flag)
{
	(void) flag;
	if (DEBUG > 2)
		std::cout << "httpConfig constructor for errors" << std::endl;
	this->mResource = config.getConfigMap()["error_page"];
	this->mResource.append(errorFile);
}

httpRequest::~httpRequest(void)
{
	if (DEBUG > 2)
        std::cout << "httpConfig destructor" << std::endl;
}

std::string httpRequest::getResource(void) {

	return (this->mResource);
}

void httpRequest::setResource(std::string defFolder, std::string errFile) {
	
	this->mResource = defFolder;
	this->mResource.append(errFile);
}
