/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 14:57:00 by anruland          #+#    #+#             */
/*   Updated: 2022/11/05 08:18:30 by anruland         ###   ########.fr       */
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
	sigaddset(&signals, SIGPIPE);
	s_action.sa_mask = signals;
	s_action.sa_flags = SA_RESTART;
	s_action.sa_handler = &si_handler_shell;
	sigaction(SIGINT, &s_action, NULL);
	sigaction(SIGPIPE, &s_action, NULL);
	signal(SIGQUIT, SIG_IGN);
}
