/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juliette-malaval <juliette-malaval@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 11:27:59 by juliette-ma       #+#    #+#             */
/*   Updated: 2026/07/10 17:05:20 by juliette-ma      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

int main(int ac, char **av) {
    try {
        parseArguments(ac, av);
        int port = atoi(av[1]);
        std::string password = av[2];
        
        // test a retirer
        std::cout << "Server listening on port " << port << "\nPassword: " << password << "\n";
        // / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
    }
    catch (std::exception& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }
    std::cout << "test1\n";
    std::string test1 = ":prefix commande param1 : param2 et 3 et 4\n";
    test_parsing(test1);
    std::cout << "\ntest2\n";
    std::string test2 = ":prefix          commande               param1 : param2            et 3 et 4\n";
    test_parsing(test2);
    std::cout << "\ntest3\n";
    std::string test3 = "comm               param1\r\n";
    test_parsing(test3);
    return EXIT_SUCCESS;
}