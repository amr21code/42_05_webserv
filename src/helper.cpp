/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 14:57:00 by anruland          #+#    #+#             */
/*   Updated: 2022/10/29 09:39:05 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "incl.hpp"

std::string ft_itoa(int nb)
{
	std::stringstream out;
	out << nb;

	return (out.str());
}

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

// execve(DEFAULT_SHELL, argp, envp);