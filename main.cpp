/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpecquer <jpecquer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 11:27:59 by juliette-ma       #+#    #+#             */
/*   Updated: 2026/07/31 13:41:15 by jpecquer         ###   ########.fr       */
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
    }
    catch (std::exception& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }
	return (EXIT_SUCCESS);
}