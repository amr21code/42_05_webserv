/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpConfig.class.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 13:46:13 by anruland          #+#    #+#             */
/*   Updated: 2022/11/02 17:09:02 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpConfig.class.hpp"

httpConfig::httpConfig(void)
{
	if (DEBUG > 2)
		std::cout << "httpConfig default constructor" << std::endl;
}

httpConfig::httpConfig(std::string configPath, int elem)
{
	if (DEBUG > 2)
		std::cout << "httpConfig constructor with path " << configPath << std::endl;
	this->mInitHttpConf();
	try
	{
		this->mReadConfig(configPath, elem);
	}
	catch(const std::logic_error& e)
	{
		throw e;
		// std::cerr << e.what() << '\n';
	}
}

httpConfig::~httpConfig(void)
{
    if (DEBUG > 2)
		std::cout << "httpConfig destructor" << std::endl;
}

void httpConfig::mReadConfig(std::string configPath, int elem)
{
	if (DEBUG > 2)
		std::cout << "readConfig with path " << configPath << std::endl;
	std::ifstream 						ss;

	std::string 						confLine;
	int									cntElem = 0;
	int									i = 0;
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
				std::map<std::string, std::string>::iterator it;
				for (it = this->mConfigMap.begin(); it != this->mConfigMap.end(); it++)
				{
					if (it->first == tmp[0])
							break ;
				}
				if (it == this->mConfigMap.end())
					throw std::logic_error("Error (1): crap inside server");
				if (this->mConfigMap[tmp[0]] == "")
					this->mConfigMap[tmp[0]] = tmp[1];
			}
			else if (confLine.find("<location>") < confLine.npos)
			{
				if (i > 0)
					this->mConfLocations.push_back(this->mStdLocation);
				while (getline(ss, confLine))
				{
					confLine.erase(std::remove(confLine.begin(), confLine.end(), '\t'), confLine.end());
					if (confLine.find("</location>") < confLine.npos) // just to skip
						break ;
					tmp = explode(confLine, ':');
					std::map<std::string, std::string>::iterator it;
					for (it = this->mConfLocations[i].begin(); it != this->mConfLocations[i].end(); it++)
					{
						if (it->first == tmp[0])
								break ;
					}
					if (it == this->mConfLocations[i].end())
						throw std::logic_error("Error (2): crap inside server");
					if (this->mConfLocations[i][tmp[0]] == "")
						this->mConfLocations[i][tmp[0]] = tmp[1];
				}
				i++;
			}
		}
		else if ((confLine.find("</server>") && cntElem == elem) || cntElem > elem)
			break ;
	}
	for (std::map<std::string, std::string>::iterator it = this->mConfigMap.begin(); it != this->mConfigMap.end(); it++)
	{
		if (it->second == "")
			it->second = this->mConfigDefault[it->first];
		// std::cout << it->first << " " << it->second << std::endl;
	}
	for (std::vector<std::map<std::string, std::string> >::iterator itv = this->mConfLocations.begin(); itv != this->mConfLocations.end(); itv++)
	{
		for (std::map<std::string, std::string>::iterator it = (*itv).begin(); it != (*itv).end(); it++)
		{
			if (it->first == "allowed_methods" && it->second == "" )
				it->second = this->mConfigMap["allowed_methods"];
			if (it->second == "")
				it->second = this->mConfigDefault[it->first];
			if (it->first == "location" && it->second[it->second.size() - 1] != '/')
				it->second.append("/");
			if (it->first == "root" && it->second[it->second.size() - 1] != '/')
				it->second.append("/");
			if (it->first == "allowed_methods")
			{
				std::vector<std::string> tmpMethods = explode(it->second, ",");
				for (std::vector<std::string>::iterator itmeth = tmpMethods.begin(); itmeth != tmpMethods.end(); itmeth++)
				{
					if ((*itmeth).compare("GET") && (*itmeth).compare("POST") && (*itmeth).compare("DELETE"))
						throw std::logic_error("Error: Invalid request method in config");
				}

			}
			// std::cout << it->first << " " << it->second << std::endl;
		}
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
	this->mStdLocation["root"] = "";
	this->mStdLocation["index"] = "";
	this->mStdLocation["autoindex"] = "";
	this->mStdLocation["redirect"] = "";
	this->mStdLocation["location"] = "";
	this->mStdLocation["allowed_methods"] = "";
	this->mStdLocation["dirlisting"] = "";
	this->mStdLocation["upload"] = "";
	this->mConfLocations.push_back(this->mStdLocation);
	
	this->mConfigDefault["server_names"] = "webserv";
	this->mConfigDefault["host"] = "0.0.0.0";
	this->mConfigDefault["port"] = "2000";
	this->mConfigDefault["error_page"] = "./www/errors/";
	this->mConfigDefault["client_max_body_size"] = "1000000";
	this->mConfigDefault["allowed_methods"] = "GET";
	this->mConfigDefault["CGI"] = "php,py";
	this->mConfigDefault["root"] = "/home/pi/projects/C05_webserv/42_05_webserv/www/html/";
	this->mConfigDefault["index"] = "index.php,index.html";
	this->mConfigDefault["autoindex"] = "1";
	this->mConfigDefault["location"] = "/";
	this->mConfigDefault["dirlisting"] = "0";
	this->mConfigDefault["redirect"] = "";
	this->mConfigDefault["upload"] = "";
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

std::vector<std::map <std::string, std::string> > &httpConfig::getConfLocations(void)
{
	return (this->mConfLocations);
}

std::map<std::string, std::string>	httpConfig::getConfigMap(void)
{
	return (this->mConfigMap);
}

std::map<std::string, std::string>	httpConfig::getDefaultMap(void)
{
	return(this->mConfigDefault);
}
// std::string httpConfig::getErrors(void)
// {
// }

size_t httpConfig::getMaxBodySize(void) {

	return(atoi(this->mConfigMap["client_max_body_size"].c_str()));	
};

// std::vector<std::string>	httpConfig::getMethods(void) {
	
// };

// std::vector<std::string>	httpConfig::getCGI(void)
// {
    
// };