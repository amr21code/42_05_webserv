/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:59:34 by anruland          #+#    #+#             */
/*   Updated: 2022/09/30 11:10:51 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpServer.hpp"
#include <pthread.h>
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
	// check for several format errors
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
	}
	if (!countServers)
		throw std::logic_error("Error: no server in config");
	return (countServers);
}

void	*startServer(void *arg)
{
	httpServer *tmp = (httpServer *)arg;
	tmp->listenSocket();
	return (0);
}

int	main(int argc, char **argv)
{
	if (argc !=2)
	{
		std::cerr << "wrong number of input arguments" << std::endl;
		return (1);
	}
	std::ifstream test(argv[1]);
    if (!test.good())
	{
		std::cerr << "could not open config file" << std::endl;
		return (1);
	}

	std::string configPath(argv[1]);
	int			countServers = 0;
	try
	{
		countServers = cfgErrorCheck(configPath);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	std::vector<httpConfig *> confVector;
	std::vector<httpServer *> serverVector;
	std::vector<pthread_t *> threadVector;
	for (int i = 0; i < countServers; i++)
	{
		confVector.push_back(new httpConfig(configPath, i + 1));
		serverVector.push_back(new httpServer(confVector[i]));
	}
	for (int i = 0; i < countServers; i++)
	{
		pthread_t *newThread = new pthread_t();
		threadVector.push_back(newThread);
		pthread_create(threadVector[i], NULL, startServer, serverVector[i]);
	}
	for (int i = 0; i < countServers; i++)
	{
		pthread_join(*threadVector[i], NULL);
	}
	for (int i = 0; i < countServers; i++)
	{
		if (confVector[i])
			delete confVector[i];
		confVector[i] = NULL;
	}
	return 0;
}
