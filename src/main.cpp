/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:59:34 by anruland          #+#    #+#             */
/*   Updated: 2022/09/28 18:56:18 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpServer.hpp"
#include <pthread.h>

int cfgLocationErrorCheck(std::string &confLine, std::ifstream &ss)
{
	if (ss.peek() == std::ifstream::traits_type::eof())
	{
		std::cerr << "Error (1): unclosed <location> element" << std::endl;
		return (-1);
	}
	while (getline(ss, confLine))
	{
		confLine.erase(std::remove(confLine.begin(), confLine.end(), '\t'), confLine.end()); // should remove tabs
		if (!confLine.compare("<server>") || !confLine.compare("</server>"))
		{
			std::cerr << "Error (4): unclosed <location> element" << std::endl;
			return (-1);
		}
		if (confLine.find("<location>") < confLine.npos)
		{
			std::cerr << "Error (2): unclosed <location> element" << std::endl;
			return (-1);
		}
		if (confLine.find("</location>") < confLine.npos)
		{
			break ;
		}
		if (ss.peek() == std::ifstream::traits_type::eof())
		{
			std::cerr << "Error (3): unclosed <location> element" << std::endl;
			return (-1);
		}
	}
	return (0);
}

int	cfgErrorCheck(std::string configPath)
{
	// check for several format errors
	std::ifstream ss;
	std::string confLine;
	int			countServers = 0;

    ss.open(configPath.c_str());

  	if (ss.peek() == std::ifstream::traits_type::eof())
    {
        std::cerr << "file is empty" << std::endl;    
		return (-1); // add exit function/exception? handle this kind of cases everywhere
    }

	while (getline(ss, confLine))
	{
		confLine.erase(std::remove(confLine.begin(), confLine.end(), '\t'), confLine.end()); // should remove tabs

		if (confLine.find("<server>") < confLine.npos)
		{
			if (ss.peek() == std::ifstream::traits_type::eof())
			{
				std::cerr << "Error (1): unclosed <server> element" << std::endl;
				return (-1);
			}
			while (getline(ss, confLine))
			{
				confLine.erase(std::remove(confLine.begin(), confLine.end(), '\t'), confLine.end()); // should remove tabs
				if (confLine.find("<server>") < confLine.npos)
				{
					std::cerr << "Error (2): unclosed <server> element" << std::endl;
					return (-1);
				}
				if (confLine.find("<location>") < confLine.npos)
				{
					if (cfgLocationErrorCheck(confLine, ss) < 0)
						return (-1);
				}
				if (confLine.find("</server>") < confLine.npos)
				{
					countServers++;
					break ;
				}
				if (ss.peek() == std::ifstream::traits_type::eof())
				{
					std::cerr << "Error (3): unclosed <server> element" << std::endl;
					return (-1);
				}
			}
		}
	}
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
	int			countServers = cfgErrorCheck(configPath);
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
