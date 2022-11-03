/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpConfig.class.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djedasch <djedasch@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 13:46:13 by anruland          #+#    #+#             */
/*   Updated: 2022/11/03 14:23:14 by djedasch         ###   ########.fr       */
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

	private:
        void	mReadConfig(std::string configPath, int elem);
		void	mInitHttpConf(void);
		std::map<std::string, std::string>					mConfigMap;
		std::map<std::string, std::string>					mConfigDefault;
		std::map<std::string, std::string> 					mStdLocation;
		std::vector<std::map <std::string, std::string> > 	mConfLocations;

};

#endif