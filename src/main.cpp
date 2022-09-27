/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anruland <anruland@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 13:59:34 by anruland          #+#    #+#             */
/*   Updated: 2022/09/27 16:41:55 by anruland         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpServer.hpp"

int	cfgErrorCheck(std::string configPath)
{
	// check for several format errors
	std::ifstream ss;
    ss.open(configPath.c_str());

  	if (ss.peek() == std::ifstream::traits_type::eof())
    {
        std::cerr << "file is empty" << std::endl;    
		return (-1); // add exit function/exception? handle this kind of cases everywhere
    }
	// std::map<std::string, std::string> configMap;
	
	// configMap["server_names"] = "";
	// configMap["host"] = "";
	// configMap["port"] = "";
	// configMap["error_page"] = "";
	// configMap["client_max_body_size"] = "";
	// configMap["allowed_methods"] = "";
	// configMap["CGI"] = "";

	std::string confLine;
	int			countServers = 0;
	while (getline(ss, confLine))
	{
		confLine.erase(std::remove(confLine.begin(), confLine.end(), '\t'), confLine.end()); // should remove tabs

		if (confLine.find("<server>") < confLine.npos)
		{
			if (ss.peek() == std::ifstream::traits_type::eof())
			{
				std::cerr << "Error (1): unclosed <server> element" << std::endl;
				return (-1);
			}
			while (getline(ss, confLine))
			{
				confLine.erase(std::remove(confLine.begin(), confLine.end(), '\t'), confLine.end()); // should remove tabs

				if (confLine.find("<server>") < confLine.npos)
				{
					std::cerr << "Error (2): unclosed <server> element" << std::endl;
					return (-1);
				}
				if (confLine.find("</server>") < confLine.npos)
				{
					countServers++;
					break ;
				}
				if (ss.peek() == std::ifstream::traits_type::eof())
				{
					std::cerr << "Error (3): unclosed <server> element" << std::endl;
					return (-1);
				}
			}
		}
		// std::cout << confLine << std::endl; // should print without tabs
	
	}
	return (countServers);
	// return number of configs if all are valid
}


int	main(int argc, char **argv)
{
	if (argc !=2)
	{
		std::cerr << "wrong number of input arguments" << std::endl;
		return (1);
	}
	std::ifstream test(argv[1]);
    if (!test.good())
	{
		std::cerr << "could not open config file" << std::endl;
		return (1);
	}

	std::string configPath(argv[1]);
	int			countServers = cfgErrorCheck(configPath);
	std::vector<httpConfig *> confVector;
	for (int i = 0; i < countServers; i++)
	{
		// httpConfig	*tmp = ;
		confVector.push_back(new httpConfig(configPath, i + 1));
		//vector of config objects
		//vector of server objects(construction with config object)
		//later with thread
	}

		// httpServer start(configPath);
	return 0;
}
