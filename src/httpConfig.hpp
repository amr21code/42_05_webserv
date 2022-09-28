/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpConfig.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 13:46:13 by anruland          #+#    #+#             */
/*   Updated: 2022/09/28 16:00:06 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPCONFIG_HPP
#define HTTPCONFIG_HPP
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <map>
#ifndef DEBUG
#define DEBUG 0
#endif

class httpConfig
{

	public:
        httpConfig(void);
        httpConfig(std::string configPath, int elem);
        ~httpConfig(void);

		std::string				    getServerNames(void);
        std::string                 getHost(void);
        std::string                 getPort(void);
        // std::string					getErrors(void);
		// int							getMaxBodySize(void);
		// std::vector<std::string>	getMethods(void);
		// std::vector<std::string>	getCGI(void);
		std::vector<std::string> explode(std::string confLine, char c);

	private:
		// std::vector<std::string>	mServerNames;
		// std::string					mHost;
		// int							mPort;
		std::map<std::string, std::string> 	mConfigMap;
		std::map<std::string, std::string> 	mConfigDefault;
		// std::string					mErrors; // filename / object
		// int							mMaxBodySize; //max 1M
		// std::vector<std::string>	mMethods;
		// std::vector<std::string>	mCGI;
        void	readConfig(std::string configPath, int elem);
		void	initHttpConf(void);

};

#endif