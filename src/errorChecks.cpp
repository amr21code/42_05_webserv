/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errorChecks.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djedasch <djedasch@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 14:15:37 by anruland          #+#    #+#             */
/*   Updated: 2022/11/03 14:33:16 by djedasch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpServer.class.hpp"
#include <exception>

/**
 * @brief check for error in config location
 * 
 * @param confLine 
 * @param ss 
 */
void cfgLocationErrorCheck(std::string &confLine, std::ifstream &ss)
{
	if (ss.peek() == std::ifstream::traits_type::eof())
		throw std::logic_error("Error (1): unclosed <location> element");
	while (getline(ss, confLine))
	{
		confLine.erase(std::remove(confLine.begin(), confLine.end(), '\t'), confLine.end());
		if (!confLine.compare("<server>") || !confLine.compare("</server>"))
			throw std::logic_error("Error (2): unclosed <location> element");
		if (confLine.find("<location>") < confLine.npos)
			throw std::logic_error("Error (3): unclosed <location> element");
		if (confLine.find("</location>") < confLine.npos)
		{
			if (confLine.compare("</location>"))
				throw std::logic_error("Error (1): only one tag per line allowed");
			break ;
		}
		if (ss.peek() == std::ifstream::traits_type::eof())
			throw std::logic_error("Error (4): unclosed <location> element");
	}
}

/**
 * @brief check for invalid configurations in config file
 * 
 * @param configPath 
 * @return int 
 */
int	cfgErrorCheck(std::string configPath)
{
	std::ifstream ss;
	std::string confLine;
	int			countServers = 0;

    ss.open(configPath.c_str());

  	if (ss.peek() == std::ifstream::traits_type::eof())
        throw std::logic_error("Error: file is empty");

	while (getline(ss, confLine))
	{
		confLine.erase(std::remove(confLine.begin(), confLine.end(), '\t'), confLine.end());

		if (confLine.find("<server>") < confLine.npos)
		{
			if (confLine.compare("<server>"))
				throw std::logic_error("Error (2): only one tag per line allowed");
			if (ss.peek() == std::ifstream::traits_type::eof())
				throw std::logic_error("Error (1): unclosed <server> element");
			while (getline(ss, confLine))
			{
				confLine.erase(std::remove(confLine.begin(), confLine.end(), '\t'), confLine.end());
				if (confLine.find("<server>") < confLine.npos)
					throw std::logic_error("Error (2): unclosed <server> element");
				if (confLine.find("<location>") < confLine.npos)
				{
					try
					{
						if (confLine.compare("<location>"))
							throw std::logic_error("Error (3): only one tag per line allowed");
						cfgLocationErrorCheck(confLine, ss);
					}
					catch(const std::logic_error& e) 	
					{
						throw e;
					}
				}
				else if (confLine.find("</location>") < confLine.npos)
					throw std::logic_error("Error: closing </location> without opening");
				if (confLine.find("</server>") < confLine.npos)
				{
					if (confLine.compare("</server>"))
						throw std::logic_error("Error (4): only one tag per line allowed");
					countServers++;
					break ;
				}
				if (ss.peek() == std::ifstream::traits_type::eof())
					throw std::logic_error("Error (3): unclosed <server> element");
			}
		}
		else if (confLine.find("</server>") < confLine.npos)
			throw std::logic_error("Error: closing </server> without opening");
		else if (confLine.find_first_not_of(" ") < confLine.npos)
			throw std::logic_error("Error: crap between servers");
	}
	if (!countServers)
		throw std::logic_error("Error: no server in config");
	return (countServers);
}

/**
 * @brief split confLine at str
 * 
 * @param confLine 
 * @param str 
 * @return std::vector<std::string> 
 */
std::vector<std::string> explode(std::string confLine, std::string str)
{
	std::vector<std::string> ret_vector;
	size_t strPosLast = 0;
	size_t strPosNext = confLine.find(str);
	
	while (strPosNext != std::string::npos)
	{
		ret_vector.push_back(confLine.substr(strPosLast, (strPosNext - strPosLast)));
		strPosLast = strPosNext + str.length();
		strPosNext = confLine.find(str, strPosLast);
	}
	ret_vector.push_back(confLine.substr(strPosLast));
	return (ret_vector);
}

/**
 * @brief split confLine at first c
 * 
 * @param confLine 
 * @param c 
 * @return std::vector<std::string> 
 */
std::vector<std::string> explode(std::string confLine, char c)
{
	std::vector<std::string> ret_vector;
	size_t colonPos = confLine.find(c);
	
	ret_vector.push_back(confLine.substr(0, colonPos));
	ret_vector.push_back(confLine.substr(colonPos + 1, confLine.length() - 1));
	return (ret_vector);
}


/**
 * @brief split confLine at first str
 * 
 * @param confLine 
 * @param str 
 * @return std::vector<std::string> 
 */
std::vector<std::string> explodeOneLine(std::string confLine, std::string str)
{
	std::vector<std::string> ret_vector;
	size_t colonPos = confLine.find(str);
	
	ret_vector.push_back(confLine.substr(0, colonPos));
	ret_vector.push_back(confLine.substr(colonPos + 1, confLine.length() - 1));
	return (ret_vector);
}