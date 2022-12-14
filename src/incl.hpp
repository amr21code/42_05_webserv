/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   incl.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/24 17:15:59 by anruland          #+#    #+#             */
/*   Updated: 2022/11/03 16:22:21 by anruland         ###   ########.fr       */
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
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include <cstdlib>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>
#include <csignal>

//errorChecks.cpp
void						cfgLocationErrorCheck(std::string &confLine, std::ifstream &ss);
int							cfgErrorCheck(std::string configPath);
std::vector<std::string>	explode(std::string confLine, std::string str);
std::vector<std::string>	explode(std::string confLine, char c);
std::vector<std::string>	explodeOneLine(std::string confLine, std::string str);
std::string 				ft_itoa(int nb);
void						ws_single_desctruct(void *str);
void						ws_destroy_array(char **argv);
void						si_handler_shell(int sig);
void						si_init_sighandling(void);

#endif