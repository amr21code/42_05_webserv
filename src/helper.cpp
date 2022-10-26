/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 14:57:00 by anruland          #+#    #+#             */
/*   Updated: 2022/10/26 15:01:14 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "incl.hpp"

std::string ft_itoa(int nb)
{
	std::stringstream out;
	out << nb;

	return (out.str());
}
