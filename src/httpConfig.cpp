/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpConfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 13:46:13 by anruland          #+#    #+#             */
/*   Updated: 2022/09/30 11:28:03 by anruland         ###   ########.fr       */
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
	this->mInitHttpConf();
	this->mReadConfig(configPath, elem);
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

void httpConfig::mReadConfig(std::string configPath, int elem)
{
	if (DEBUG > 2)
		std::cout << "readConfig with path" << configPath << std::endl;
	std::ifstream 						ss;

	std::string 						confLine;
	int									cntElem = 0;
	std::vector<std::string> 			tmp;


	
    ss.open(configPath.c_str());

	while (getline(ss, confLine))
	{
		if (confLine.find("<server>") < confLine.npos)
			cntElem++;
		else if (cntElem == elem)
		{
			confLine.erase(std::remove(confLine.begin(), confLine.end(), '\t'), confLine.end());

			if (confLine.find_first_of(':') < confLine.npos)
			{
				tmp = explode(confLine, ':');
				if (this->mConfigMap[tmp[0]] == "")
					this->mConfigMap[tmp[0]] = tmp[1];
			}
			else if (confLine.find("<location>") < confLine.npos)
			{
				while (getline(ss, confLine))
				{
					if (confLine.find("</location>") < confLine.npos) // just to skip
						break ;
				}
			}
		}
		else if ((confLine.find("</server>") && cntElem == elem) || cntElem > elem)
			break ;
	}
	for (std::map<std::string, std::string>::iterator it = this->mConfigMap.begin(); it != this->mConfigMap.end(); it++)
	{
		if (it->second == "")
			it->second = this->mConfigDefault[it->first];
		std::cout << it->first << " " << it->second << std::endl;
	}
}

void	httpConfig::mInitHttpConf(void)
{
	this->mConfigMap["server_names"] = "";
	this->mConfigMap["host"] = "";
	this->mConfigMap["port"] = "";
	this->mConfigMap["error_page"] = "";
	this->mConfigMap["client_max_body_size"] = "";
	this->mConfigMap["allowed_methods"] = "";
	this->mConfigMap["CGI"] = "";
	// this->mConfigMap["root"] = "";
	// this->mConfigMap["index"] = "";
	// this->mConfigMap["autoindex"] = "";
	// this->mConfigMap["redirect"] = "";
	
	this->mConfigDefault["server_names"] = "webserv";
	this->mConfigDefault["host"] = "0.0.0.0";
	this->mConfigDefault["port"] = "80";
	this->mConfigDefault["error_page"] = "./www/errors";
	this->mConfigDefault["client_max_body_size"] = "1000000";
	this->mConfigDefault["allowed_methods"] = "GET,POST,DELETE";
	this->mConfigDefault["CGI"] = "php,py";
	// this->mConfigDefault["root"] = "/home/pi/projects/C05_webserv/42_05_webserv/www/html/";
	// this->mConfigDefault["index"] = "index.php,index.html";
	// this->mConfigDefault["autoindex"] = "1";
	// this->mConfigDefault["redirect"] = "return 301 index.html https://www.google.com/";
}

std::string httpConfig::getServerNames(void)
{
    return (this->mConfigMap["server_names"]);
}

std::string httpConfig::getHost(void)
{
    return (this->mConfigMap["host"]);
}

int httpConfig::getPort(void)
{
    return (atoi(this->mConfigMap["port"].c_str()));
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