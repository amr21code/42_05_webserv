/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djedasch <djedasch@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:59:34 by anruland          #+#    #+#             */
/*   Updated: 2022/11/04 09:29:00 by djedasch         ###   ########.fr       */
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
			if (event_count > 0) // && !(errno == EAGAIN || errno == EWOULDBLOCK))
			{
				for(int i = 0; i < event_count; i++)
				{
					// std::cout << "i " << i << " ev cnt " << event_count << std::endl;
					// std::cout<< "servers " << countServers << std::endl;
					std::cout<< "data.fd "<< epevents[i].data.fd << std::endl;
					for (int j = 0; j < countServers; j++)
					{
						std::cout<< "socket " << countServers << serverVector[j]->getSocket() << std::endl;
						std::cout <<" message size " << serverVector[j]->getMsg()[epevents[i].data.fd].size() << std::endl;
						if (serverVector[j]->getSocket() == epevents[i].data.fd)
						{
							std::cout << "ACCEPT SOCKET" << std::endl;
							if (epevents[i].events & EPOLLIN)
							{
								tmpfd = serverVector[j]->acceptSocket();
								std::cout<< "tmpfd " << tmpfd << std::endl;
								if (tmpfd > 2)
								{
									epevent.data.fd = tmpfd;
									if (serverVector[j]->receive(tmpfd))
									{
										serverVector[j]->eraseMsg(tmpfd);
										break;
									}
									if (epoll_ctl(epfd, EPOLL_CTL_ADD, tmpfd, &epevent))
										throw std::logic_error("Error (2): Failed to add file descriptor to epoll");
									if (serverVector[j]->generateRequest(tmpfd))
									{
										if (epoll_ctl(epfd, EPOLL_CTL_DEL, tmpfd, &epevent))
											throw std::logic_error("Error: Failed to delete file descriptor to epoll");
										if (!close(tmpfd))
											std::cout << "fd closed " << tmpfd << std::endl;
										serverVector[j]->eraseMsg(tmpfd);
									}
								}
							}
						}
						else
						{
							std::cout << "SEND/RECV TO FD" << std::endl;
							std::map<int, std::string> tmpMap = serverVector[j]->getMsg();
							for (std::map<int, std::string>::iterator itmsg = tmpMap.begin(); itmsg != tmpMap.end(); itmsg++)
							{
								// std::cout<< "1 it first " << itmsg->first<< " epevents " << epevents[i].data.fd << std::endl;
								if (itmsg->first == epevents[i].data.fd)
								{
									std::cout<< "2 it first " << itmsg->first<< " epevents " << epevents[i].data.fd << std::endl;
									std::cout << epevents[i].events << std::endl;
									if (epevents[i].events & EPOLLIN)
									{
										if (serverVector[j]->receive(epevents[i].data.fd))
										{
											if (epoll_ctl(epfd, EPOLL_CTL_DEL, epevents[i].data.fd, &epevent))
												throw std::logic_error("Error: Failed to delete file descriptor to epoll");
											serverVector[j]->eraseMsg(epevents[i].data.fd);
											serverVector[j]->eraseRequest(epevents[i].data.fd);
										}
										std::cout << "TEST EPOLLIN" << std::endl;
									}
									else if ((epevents[i].events & EPOLLOUT) && serverVector[j]->readyToWrite(epevents[i].data.fd))
									{
										std::cout<< "3 it first " << itmsg->first<< " epevents " << epevents[i].data.fd << std::endl;
										if (serverVector[j]->generateRequest(tmpfd))
											std::cerr << "Error: generate Request failed" << std::endl;
										else
											serverVector[j]->answer(epevents[i].data.fd);
										//wenn  epevents[i].data.fd == msgfd aus vector -> index an answer
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