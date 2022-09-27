/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpConfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 13:46:13 by anruland          #+#    #+#             */
/*   Updated: 2022/09/27 17:11:51 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpConfig.hpp"

httpConfig::httpConfig(void)
{
	if (DEBUG > 2)
		std::cout << "httpConfig default constructor" << std::endl;
}

httpConfig::httpConfig(std::string configPath, int elem)
{
	if (DEBUG > 2)
		std::cout << "httpConfig constructor with path" << configPath << std::endl;
	readConfig(configPath, elem);
}

httpConfig::~httpConfig(void)
{
    if (DEBUG > 2)
		std::cout << "httpConfig destructor" << std::endl;
}

std::vector<std::string> httpConfig::explode(std::string confLine, char c)
{
	std::vector<std::string> ret_vector;

	size_t colonPos = confLine.find(c);
	
	ret_vector.push_back(confLine.substr(0, colonPos));
	ret_vector.push_back(confLine.substr(colonPos + 1, confLine.length() - 1));

	
	return (ret_vector);
}

void httpConfig::readConfig(std::string configPath, int elem)
{
	if (DEBUG > 2)
		std::cout << "readConfig with path" << configPath << std::endl;
	std::ifstream 						ss;
	std::map<std::string, std::string> 	configMap;
	std::string 						confLine;
	int									cntElem = 0;
	std::vector<std::string> 			tmp;

	configMap["server_names"] = "";
	configMap["host"] = "";
	configMap["port"] = "";
	configMap["error_page"] = "";
	configMap["client_max_body_size"] = "";
	configMap["allowed_methods"] = "";
	configMap["CGI"] = "";
	
    ss.open(configPath.c_str());

	while (getline(ss, confLine))
	{
		if (confLine.find("<server>") < confLine.npos)
			cntElem++;
		else if (cntElem == elem)
		{
			confLine.erase(std::remove(confLine.begin(), confLine.end(), '\t'), confLine.end());

			if (confLine.find(':') < confLine.npos)
			{
				tmp = explode(confLine, ':');
				if (configMap[tmp[0]] == "")
					configMap[tmp[0]] = tmp[1];
			}
		}
		else if (cntElem > elem)
			break ;
	}


// auslagern in setDefault
	std::map<std::string, std::string> 	configDefault;
	configDefault["server_names"] = "webserv";
	configDefault["host"] = "0.0.0.0";
	configDefault["port"] = "80";
	configDefault["error_page"] = "./www/errors";
	configDefault["client_max_body_size"] = "1000000";
	configDefault["allowed_methods"] = "GET,POST,DELETE";
	configDefault["CGI"] = "php,py";
	for (std::map<std::string, std::string>::iterator it = configMap.begin(); it != configMap.end(); it++)
	{
		if (it->second == "")
			it->second = configDefault[it->first];
		std::cout << it->first << " " << it->second << std::endl;

	}
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