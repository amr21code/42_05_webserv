/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpConfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 13:46:13 by anruland          #+#    #+#             */
/*   Updated: 2022/09/26 16:58:23 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpConfig.hpp"

httpConfig::httpConfig(void)
{
	if (DEBUG > 2)
		std::cout << "httpConfig default constructor" << std::endl;
}

httpConfig::httpConfig(std::string configPath)
{
	if (DEBUG > 2)
		std::cout << "httpConfig constructor with path" << configPath << std::endl;
	readConfig(configPath);
}

httpConfig::~httpConfig(void)
{
    if (DEBUG > 2)
		std::cout << "httpConfig destructor" << std::endl;
}

void httpConfig::readConfig(std::string configPath)
{
	if (DEBUG > 2)
		std::cout << "readConfig with path" << configPath << std::endl;
	std::stringstream ss(configPath.c_str());

  	if (ss.peek() == std::ifstream::traits_type::eof())
    {
        std::cerr << "file is empty" << std::endl;    
		return ; // add exit function/exception? handle this kind of cases everywhere
    }

	std::string confLine;
	while (getline(ss, confLine))
	{
		confLine.erase(std::remove(confLine.begin(), confLine.end(), '\t'), confLine.end()); // should remove tabs
		std::cout << confLine << std::endl; // should print without tabs
	}

	// this->mServerNames
	// this->mHost
	// this->mPort
}

std::vector<std::string> httpConfig::getServerNames(void)
{
    return (this->mServerNames);
}

std::string httpConfig::getHost(void)
{
    return (this->mHost);
}

int httpConfig::getPort(void)
{
    return (this->mPort);
}

// std::string httpConfig::getErrors(void)
// {
// }

// int httpConfig::getMaxBodySize(void) {
	
// };

// std::vector<std::string>	httpConfig::getMethods(void) {
	
// };

// std::vector<std::string>	httpConfig::getCGI(void)
// {
    
// };