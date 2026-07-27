/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juliette-malaval <juliette-malaval@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 11:27:59 by juliette-ma       #+#    #+#             */
/*   Updated: 2026/07/27 15:56:01 by juliette-ma      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "server.hpp"

int main(int ac, char **av) {
    try {
        parseArguments(ac, av);
        unsigned int port = atoi(av[1]);
        std::string password = av[2];
        serv	server(port, password);
    }
    catch (std::exception& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }
	return (EXIT_SUCCESS);
}