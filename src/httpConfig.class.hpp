/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpConfig.class.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 13:46:13 by anruland          #+#    #+#             */
/*   Updated: 2022/10/26 14:51:38 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPCONFIG_CLASS_HPP
#define HTTPCONFIG_CLASS_HPP

#include "incl.hpp"
#ifndef DEBUG
#define DEBUG 0
#endif

class httpConfig
{

	public:
        httpConfig(void);
        httpConfig(std::string configPath, int elem);
        ~httpConfig(void);

		std::string				   							getServerNames(void);
        std::string											getHost(void);
        int													getPort(void);
		std::vector<std::map <std::string, std::string> >	&getConfLocations(void);
		std::map<std::string, std::string>					getConfigMap(void);
		std::map<std::string, std::string>					getDefaultMap(void);
		size_t												getMaxBodySize(void);
        // std::string					getErrors(void);
		// int							getMaxBodySize(void);
		// std::vector<std::string>	getMethods(void);
		// std::vector<std::string>	getCGI(void);
		// std::vector<std::string> explode(std::string confLine, char c);

	private:
		// std::vector<std::string>	mServerNames;
		// std::string					mHost;
		// int							mPort;
		std::map<std::string, std::string>					mConfigMap;
		std::map<std::string, std::string>					mConfigDefault;
		std::map<std::string, std::string> 					mStdLocation;
		std::vector<std::map <std::string, std::string> > 	mConfLocations;
		// std::string					mErrors; // filename / object
		// int							mMaxBodySize; //max 1M
		// std::vector<std::string>	mMethods;
		// std::vector<std::string>	mCGI;
        void	mReadConfig(std::string configPath, int elem);
		void	mInitHttpConf(void);

};

#endif