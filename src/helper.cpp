/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djedasch <djedasch@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 14:57:00 by anruland          #+#    #+#             */
/*   Updated: 2022/11/03 14:47:02 by djedasch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "incl.hpp"

/**
 * @brief helper for itoa process
 */
std::string ft_itoa(int nb)
{
	std::stringstream out;
	out << nb;

	return (out.str());
}

/**
 * @brief frees string if it exists 
 */
void	ws_single_desctruct(void *str)
{
	if (str)
		free(str);
	str = NULL;
}

/**
 * frees a pointer to pointer
 * @param argv [char **] pointer to pointer to free
 */
void	ws_destroy_array(char **argv)
{
	int	i;

	i = 0;
	while (argv[i])
	{
		ws_single_desctruct(argv[i]);
		i++;
	}
	delete[] argv;
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

/**
 * @brief handles signals
 * 
 */
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