/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpConfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 13:46:13 by anruland          #+#    #+#             */
/*   Updated: 2022/09/26 13:55:15 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpConfig.hpp"

httpConfig::httpConfig()
{
	if (DEBUG > 2)
		std::cout << "httpConfig default constructor" << std::endl;
}
httpConfig::~httpConfig()
{
    if (DEBUG > 2)
		std::cout << "httpConfig destructor" << std::endl;
}