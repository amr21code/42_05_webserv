/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:59:34 by anruland          #+#    #+#             */
/*   Updated: 2022/10/11 16:04:55 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpServer.class.hpp"
#include <exception>
#include <sys/epoll.h>

void	destroyAllocs(std::vector<httpConfig *> confVector, std::vector<httpServer *> serverVector, int actualServers, int epfd)
{
	for (int i = 0; i < actualServers; i++)
	{
		if (confVector[i])
			delete confVector[i];
		confVector[i] = NULL;
		if (serverVector[i])
			delete serverVector[i];
		serverVector[i] = NULL;
	}
	try
	{
		if (close(epfd))
			throw std::logic_error("Error: Failed to close epoll file descriptor");
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
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
	std::vector<httpConfig *> 	confVector;
	std::vector<httpServer *> 	serverVector;
	int							epfd = epoll_create(1);
	struct epoll_event			epevent;
	epevent.events = EPOLLIN;
	try
	{
		if (epfd == -1)
			throw std::logic_error("Error: Failed to create epoll file descriptor");
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	for (int i = 0; i < countServers; i++)
	{
		try
		{
			epevent.data.u32 = i;
			confVector.push_back(new httpConfig(configPath, i + 1));
			serverVector.push_back(new httpServer(confVector[i]));
			serverVector[i]->listenSocket();
			if (epoll_ctl(epfd, EPOLL_CTL_ADD, serverVector[i]->getSocket(), &epevent))
				throw std::logic_error("Error: Failed to add file descriptor to epoll");
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			destroyAllocs(confVector, serverVector, i, epfd);
			return (0);
		}
	}
	// struct epoll_event			epevent[countServers];
	int	event_count = 0;
	while (1)
	{
		event_count = epoll_wait(epfd, &epevent, countServers, 30000);
		std::cout << event_count << std::endl;
		serverVector[epevent.data.u32]->receive();
	}

	// while (1)
	// {
	// 	printf("\nPolling for input...\n");
	// 	printf("%d ready events\n", event_count);
	// 	for (int i = 0; i < event_count; i++) {
	// 		printf("Reading file descriptor '%d' -- ", epevent[i].data.u32);
	// 		bytes_read = read(events[i].data.fd, read_buffer, READ_SIZE);
	// 		printf("%zd bytes read.\n", bytes_read);
	// 		read_buffer[bytes_read] = '\0';
	// 		printf("Read '%s'\n", read_buffer);
		
	// 		if(!strncmp(read_buffer, "stop\n", 5))
	// 		running = 0;
	// }
	// serverVector[0]->listenSocket();

	destroyAllocs(confVector, serverVector, countServers, epfd);
	return 0;
}

