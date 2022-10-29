/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:59:34 by anruland          #+#    #+#             */
/*   Updated: 2022/10/29 08:40:51 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpServer.class.hpp"
#include <exception>

bool gShutdown = false;

void	si_handler_shell(int sig)
{
	if (sig == SIGINT)
	{
		gShutdown = true;
	}
}

void	si_init_sighandling(void)
{
	sigset_t			signals;
	struct sigaction	s_action;

	sigemptyset(&signals);
	sigaddset(&signals, SIGQUIT);
	sigaddset(&signals, SIGINT);
	s_action.sa_mask = signals;
	s_action.sa_flags = SA_RESTART;
	s_action.sa_handler = &si_handler_shell;
	sigaction(SIGINT, &s_action, NULL);
	signal(SIGQUIT, SIG_IGN);
}

void	destroyAllocs(std::vector<httpConfig *> confVector, std::vector<httpServer *> serverVector, int actualServers, int epfd)
{
	if (DEBUG > 2)
		std::cout << "main destroyAllocs" << std::endl;
	for (long unsigned int i = 0; i < static_cast<long unsigned int> (actualServers); i++)
	{
		if (confVector.size() > i)
			delete confVector[i];
		if (serverVector.size() > i)
			delete serverVector[i];
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
	if (DEBUG > 2)
		std::cout << "main startServer" << std::endl;
	httpServer *tmp = (httpServer *)arg;
	tmp->listenSocket();
	return (0);
}



int	main(int argc, char **argv)
{
	si_init_sighandling();
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
	test.close();
	std::string configPath(argv[1]);
	int			countServers = 0;
	try
	{
		countServers = cfgErrorCheck(configPath);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (0);
	}
	std::vector<httpConfig *> 	confVector;
	std::vector<httpServer *> 	serverVector;
	int							epfd = epoll_create(1);
	struct epoll_event			epevent;
	epevent.events = EPOLLIN; // |EPOLLOUT; // check later, if uploading files works without EPOLLOUT
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
			epevent.data.fd = -1;
			epevent.data.u64 = 0;
			epevent.data.ptr = NULL;
			confVector.push_back(new httpConfig(configPath, i + 1));
			serverVector.push_back(new httpServer(confVector[i]));
			serverVector[i]->listenSocket();
			if (epoll_ctl(epfd, EPOLL_CTL_ADD, serverVector[i]->getSocket(), &epevent))
				throw std::logic_error("Error: Failed to add file descriptor to epoll");
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			destroyAllocs(confVector, serverVector, i + 1, epfd);
			return (1);
		}
	}
	struct epoll_event			epevents[countServers];
	int	event_count = 0;
	while (!gShutdown)
	{
		event_count = epoll_wait(epfd, epevents, 64, -1);
		// std::cout << event_count << std::endl;
		try
		{
			if (event_count < 0 && !gShutdown)
				throw std::logic_error("Error: epoll_wait() failed");
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
		// std::cout << epevent.events << std::endl;
		if (event_count > 0) // && !(errno == EAGAIN || errno == EWOULDBLOCK))
		{
			for(int i = 0; i < event_count; i++)
			{
				//std::cout<< "servers " << countServers <<" data.u32 "<< epevents[i].data.u32 << std::endl;
				//std::cout<< "socket " << countServers <<serverVector[epevents[i].data.u32]->getSocket() << std::endl;
				serverVector[epevents[i].data.u32]->receive();
			}
		}
	}
	destroyAllocs(confVector, serverVector, countServers, epfd);
	return 0;
}
