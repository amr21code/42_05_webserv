/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:59:34 by anruland          #+#    #+#             */
/*   Updated: 2022/11/05 10:01:28 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "incl.hpp"
#include "httpServer.class.hpp"
#include <exception>

bool gShutdown = false;

void	destroyAllocs(std::vector<httpConfig *> confVector, std::vector<httpServer *> serverVector, int actualServers, int epfd);

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
	int							epfd = epoll_create(4000);
	struct epoll_event			epevent;
	epevent.events = EPOLLIN|EPOLLOUT; 
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
			epevent.data.u64 = 42;
			epevent.data.ptr = NULL;
			confVector.push_back(new httpConfig(configPath, i + 1));
			serverVector.push_back(new httpServer(confVector[i]));
			epevent.data.fd = serverVector[i]->getSocket();
			serverVector[i]->listenSocket();
			if (epoll_ctl(epfd, EPOLL_CTL_ADD, serverVector[i]->getSocket(), &epevent))
				throw std::logic_error("Error (1): Failed to add file descriptor to epoll");
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
	int	tmpfd = -1;
	while (!gShutdown)
	{
		event_count = epoll_wait(epfd, epevents, 200, 1000);
		try
		{
			if (event_count < 0 && !gShutdown)
				throw std::logic_error("Error: epoll_wait() failed");
			if (event_count > 0) 
			{
				for(int i = 0; i < event_count; i++)
				{
					for (int j = 0; j < countServers; j++)
					{
						if (serverVector[j]->getSocket() == epevents[i].data.fd)
						{
							if (epevents[i].events & EPOLLIN)
							{
								tmpfd = serverVector[j]->acceptSocket();
								if (tmpfd > 2)
								{
									epevent.data.fd = tmpfd;
									if (serverVector[j]->receive(tmpfd))
									{
										serverVector[j]->eraseMsg(tmpfd);
										close(tmpfd);
										break;
									}
									if (epoll_ctl(epfd, EPOLL_CTL_ADD, tmpfd, &epevent))
										throw std::logic_error("Error (2): Failed to add file descriptor to epoll");
									if (serverVector[j]->generateRequest(tmpfd))
									{
										serverVector[j]->eraseMsg(tmpfd);
										serverVector[j]->eraseRequest(tmpfd);
										if (epoll_ctl(epfd, EPOLL_CTL_DEL, tmpfd, &epevent))
											throw std::logic_error("Error (1): Failed to delete file descriptor to epoll");
										if (!close(tmpfd) && DEBUG > 2)
											std::cout << "fd closed " << tmpfd << std::endl;
									}
								}
							}
						}
						else
						{
							std::map<int, std::string> tmpMap = serverVector[j]->getMsg();
							for (std::map<int, std::string>::iterator itmsg = tmpMap.begin(); itmsg != tmpMap.end(); itmsg++)
							{
								if (itmsg->first == epevents[i].data.fd)
								{
									if (epevents[i].events & EPOLLIN)
									{
										if (serverVector[j]->receive(epevents[i].data.fd))
										{
											serverVector[j]->eraseMsg(epevents[i].data.fd);
											serverVector[j]->eraseRequest(epevents[i].data.fd);
											if (epoll_ctl(epfd, EPOLL_CTL_DEL, epevents[i].data.fd, &epevent))
												throw std::logic_error("Error (2): Failed to delete file descriptor to epoll");
											close(epevents[i].data.fd);
										}
									}
									else if ((epevents[i].events & EPOLLOUT) && serverVector[j]->readyToWrite(epevents[i].data.fd))
									{
										if (serverVector[j]->generateRequest(tmpfd))
											std::cerr << "Error: generate Request failed" << std::endl;
										else
											serverVector[j]->answer(epevents[i].data.fd);
										serverVector[j]->eraseRequest(epevents[i].data.fd);
										serverVector[j]->eraseMsg(epevents[i].data.fd);
										if (epoll_ctl(epfd, EPOLL_CTL_DEL, epevents[i].data.fd, &epevent))
											throw std::logic_error("Error (3): Failed to delete file descriptor to epoll");
										if (!close(epevents[i].data.fd) && DEBUG > 2)
											std::cout << "fd closed " << epevents[i].data.fd << std::endl;
									}
								}
							}
						}
					}
				}
			}
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
	destroyAllocs(confVector, serverVector, countServers, epfd);
	return 0;
}


/**
 * @brief sets gShutdown to true if pressed ctrl+c
 */
void	si_handler_shell(int sig)
{
	if (sig == SIGINT)
	{
		gShutdown = true;
	}
	else if (sig == SIGPIPE)
	{
		std::cout << "Error: File descriptor closed unexpectedly during writing" << std::endl;
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


/**
 * @brief frees memory when called
 */
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