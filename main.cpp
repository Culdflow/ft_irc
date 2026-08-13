#include "include/ft_irc.hpp"
#include "include/Server.hpp"
#include <cctype> 


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