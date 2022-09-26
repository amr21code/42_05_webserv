/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpConfig.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 13:46:13 by anruland          #+#    #+#             */
/*   Updated: 2022/09/26 13:57:23 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPCONFIG_HPP
#define HTTPCONFIG_HPP
#include <string>
#include <vector>

class httpConfig
{

	public:
        httpConfig();
        ~httpConfig();

	private:
		std::vector<std::string>	mServerNames;
		std::string					mHost;
		int							mPort;
		// std::string					mErrors;
		// int							mMaxBodySize;
		// std::vector<std::string>	mMethods;
		// std::vector<std::string>	mCGI;

};



#endif