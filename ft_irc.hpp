/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juliette-malaval <juliette-malaval@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 11:01:47 by juliette-ma       #+#    #+#             */
/*   Updated: 2026/07/10 16:48:43 by juliette-ma      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_HPP
#define FT_IRC_HPP

#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <string>
#include <vector>

struct Message {
    std::string prefix;             
    std::string command;           
    std::vector<std::string> params; 
};

// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
//struct a modifier, uniquement pour avancer sur le parsing
struct Client {
    int fd_socket;
    std::string inputBuf;
    
};

void test_parsing(std::string& s);

// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /


// parsing
void parseArguments(int ac, char **av);

#endif