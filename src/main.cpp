/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:59:34 by anruland          #+#    #+#             */
/*   Updated: 2022/10/11 14:19:17 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpServer.class.hpp"
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

