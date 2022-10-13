/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpRequest.class.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djedasch <djedasch@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 16:49:53 by anruland          #+#    #+#             */
/*   Updated: 2022/10/13 16:10:22 by djedasch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpRequest.class.hpp"

httpRequest::httpRequest(void)
{
	if (DEBUG > 2)
		std::cout << "httpConfig default constructor" << std::endl; 
}
httpRequest::httpRequest(std::string msg)
{
	if (DEBUG > 2)
		std::cout << "httpConfig constructor with message" << std::endl;
	std::cout << "msg\n" << msg << std::endl;
	// std::cout << msg.find("HTTP/1.1", 0, msg.find_first_of('\n') + 1) << std::endl;
	if (msg.find("HTTP/1.1", 0, 8) == std::string::npos)
		throw std::logic_error("403");
	
	// std::stringstream ss;
	// ss << msg;
	// std::string token;
	// while(std::getline(ss, token, ' ')) {
	// 	std::cout << token << '\n';
	// }
}

httpRequest::~httpRequest(void)
{
	if (DEBUG > 2)
        std::cout << "httpConfig destructor" << std::endl;
}
