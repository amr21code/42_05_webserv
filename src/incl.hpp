/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   incl.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/24 17:15:59 by anruland          #+#    #+#             */
/*   Updated: 2022/10/22 14:06:06 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCL_HPP
# define INCL_HPP
# define C_DEF "\033[0m"
# define C_GREY "\033[2;30m"
# define C_RED "\033[31m"
# define C_GREEN "\033[32m"
# define C_YELLOW "\033[33m"
# define C_BLUE "\033[34m"
# define C_MAGENTA "\033[35m"
# define C_WHITE "\033[37m"
#ifndef DEBUG
#define DEBUG 0
#endif

#include <vector>

enum RequestType
{
	GET = 0,
	POST,
	DELETE
};

//errorChecks.cpp
void						cfgLocationErrorCheck(std::string &confLine, std::ifstream &ss);
int							cfgErrorCheck(std::string configPath);
std::vector<std::string>	explode(std::string confLine, char c);


#endif