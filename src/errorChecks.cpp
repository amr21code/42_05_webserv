/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errorChecks.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 14:15:37 by anruland          #+#    #+#             */
/*   Updated: 2022/10/11 14:17:30 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpServer.class.hpp"
#include <exception>

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