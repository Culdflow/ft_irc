/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juliette-malaval <juliette-malaval@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 11:27:59 by juliette-ma       #+#    #+#             */
/*   Updated: 2026/07/20 17:21:27 by juliette-ma      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "server.hpp"

int main(int ac, char **av) {
    // try {
    //     parseArguments(ac, av);
    //     int port = atoi(av[1]);
    //     std::string password = av[2];
    // }
    // catch (std::exception& e) {
    //     std::cerr << e.what() << "\n";
    //     return EXIT_FAILURE;
    // }
	if (ac != 3)
		return (EXIT_FAILURE);
	serv	server(av[1], av[2]);
	return (EXIT_SUCCESS);
}