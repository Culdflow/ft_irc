/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juliette-malaval <juliette-malaval@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 11:27:59 by juliette-ma       #+#    #+#             */
/*   Updated: 2026/07/30 16:07:49 by juliette-ma      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "Server.hpp"

int main(int ac, char **av) {
    try {
        parseArguments(ac, av);
        unsigned int port = atoi(av[1]);
        std::string password = av[2];
        serv	server(port, password);
        server.run();
        //std::map<std::string, Channel>::iterator it = server.getChannelList().find("42");
        //it->second.
    }
    catch (std::exception& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }
	return (EXIT_SUCCESS);
}