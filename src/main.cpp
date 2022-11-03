/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djedasch <djedasch@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:59:34 by anruland          #+#    #+#             */
/*   Updated: 2022/11/03 14:47:37 by djedasch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpServer.class.hpp"
#include <exception>

bool gShutdown = false;

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
	int							epfd = epoll_create(256);
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
		event_count = epoll_wait(epfd, epevents, 64, 1000);
		std::cout << event_count << std::endl;
		try
		{
			if (event_count < 0 && !gShutdown)
				throw std::logic_error("Error: epoll_wait() failed");
			// std::cout << epevent.events << std::endl;
			if (event_count > 0) // && !(errno == EAGAIN || errno == EWOULDBLOCK))
			{
				for(int i = 0; i < event_count; i++)
				{
					// std::cout << "i " << i << " ev cnt " << event_count << std::endl;
					// std::cout<< "servers " << countServers << std::endl;
					// std::cout<< "data.fd "<< epevents[i].data.fd << std::endl;
					for (int j = 0; j < countServers; j++)
					{
						// std::cout<< "socket " << countServers << serverVector[j]->getSocket() << std::endl;
						if (serverVector[j]->getSocket() == epevents[i].data.fd)
						{
							std::cout << "TEST1" << std::endl;
							if (epevents[i].events == EPOLLIN)
							{
								tmpfd = serverVector[j]->receive();
								// std::cout<< "tmpfd " << tmpfd << std::endl;
								if (tmpfd > 2)
								{
									epevent.data.fd = tmpfd;
									if (epoll_ctl(epfd, EPOLL_CTL_ADD, tmpfd, &epevent))
										throw std::logic_error("Error (2): Failed to add file descriptor to epoll");
								}
								// goto new_event;
							}
						}
						else if (serverVector[j]->getMsg().size() > 13)
						{
							// std::cout << "TEST2" << std::endl;
							std::map<int, std::string> tmpMap = serverVector[j]->getMsg();
							for (std::map<int, std::string>::iterator itmsg = tmpMap.begin(); itmsg != tmpMap.end(); itmsg++)
							{
								// std::cout<< "1 it first " << itmsg->first<< " epevents " << epevents[i].data.fd << std::endl;
								if (itmsg->first == epevents[i].data.fd)
								{
								// std::cout<< "2 it first " << itmsg->first<< " epevents " << epevents[i].data.fd << std::endl;
									if (epevents[i].events == EPOLLOUT)
									{
										if (serverVector[j]->generateRequest(itmsg->first))
											break ;
										//wenn server->request->content length > msg.size -> break;

										// std::cout<< "3 it first " << itmsg->first<< " epevents " << epevents[i].data.fd << std::endl;
										//wenn  epevents[i].data.fd == msgfd aus vector -> index an answer
										serverVector[j]->answer(epevents[i].data.fd);
										// std::cout<< "4 it first " << itmsg->first<< " epevents " << epevents[i].data.fd << std::endl;
										if (epoll_ctl(epfd, EPOLL_CTL_DEL, epevents[i].data.fd, &epevent))
											throw std::logic_error("Error: Failed to delete file descriptor to epoll");
										if (!close(epevents[i].data.fd))
											std::cout << "fd closed " << epevents[i].data.fd << std::endl;
										serverVector[j]->eraseMsg(epevents[i].data.fd);
										// goto new_event;
										// break;
									}
								}
							}
						}
						else
						{
							//! error weil message zu kurz?
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
