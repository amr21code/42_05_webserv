/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:59:34 by anruland          #+#    #+#             */
/*   Updated: 2022/09/30 14:42:39 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpServer.hpp"
#include <pthread.h>
#include <exception>

void	destroyAllocs(std::vector<httpConfig *> confVector, std::vector<httpServer *> serverVector, std::vector<pthread_t *> threadVector, int countServers, int actualServers)
{
	for (int i = 0; i < actualServers; i++)
	{
		if (confVector[i])
			delete confVector[i];
		confVector[i] = NULL;
		if (serverVector[i])
			delete serverVector[i];
		serverVector[i] = NULL;
		if (countServers == actualServers)
		{
			if (threadVector[i])
				delete threadVector[i];
			threadVector[i] = NULL;
		}
	}	
}

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
		std::cerr << "Error: wrong number of input arguments" << std::endl;
		return (1);
	}
	std::ifstream test(argv[1]);
    if (!test.good())
	{
		std::cerr << "Error: could not open config file" << std::endl;
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
		try
		{
			confVector.push_back(new httpConfig(configPath, i + 1));
			serverVector.push_back(new httpServer(confVector[i]));
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			destroyAllocs(confVector, serverVector, threadVector, countServers, i);
			return (0);
		}
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
	destroyAllocs(confVector, serverVector, threadVector, countServers, countServers);
	return 0;
}

