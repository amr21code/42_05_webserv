/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpConfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 13:46:13 by anruland          #+#    #+#             */
/*   Updated: 2022/09/26 17:58:25 by anruland         ###   ########.fr       */
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
	std::ifstream ss;
    ss.open(configPath.c_str());

  	if (ss.peek() == std::ifstream::traits_type::eof())
    {
        std::cerr << "file is empty" << std::endl;    
		return ; // add exit function/exception? handle this kind of cases everywhere
    }
    std::string configVars[7] ={"server_names",	"host",	"port",	"error_page", "client_max_body_size", "allowed_methods", "CGI"};
	std::map<std::string, std::string> configMap;
	
	configMap["server_names"] = "";
	configMap["host"] = "";
	configMap["port"] = "";
	configMap["error_page"] = "";
	configMap["client_max_body_size"] = "";
	configMap["allowed_methods"] = "";
	configMap["CGI"] = "";

	std::string confLine;
	while (getline(ss, confLine))
	{
		std::vector<std::string> tmp;
		confLine.erase(std::remove(confLine.begin(), confLine.end(), '\t'), confLine.end()); // should remove tabs
		if (confLine.find(':') < confLine.npos)
		{
            tmp = explode(confLine, ':');
			std::cout << "1 " << tmp[0] << std::endl;
			std::cout << "2 " << tmp[1] << std::endl;
		}
		// std::cout << confLine << std::endl; // should print without tabs
        
		
		// for (int i = 0; i<7; i++)
        // {
        //     if ()
        // }
        // switch ()
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