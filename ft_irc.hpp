/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juliette-malaval <juliette-malaval@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 11:01:47 by juliette-ma       #+#    #+#             */
/*   Updated: 2026/07/27 16:49:32 by juliette-ma      ###   ########.fr       */
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

#include "Client.hpp"

struct Message {
    std::string prefix;             
    std::string command;           
    std::vector<std::string> params; 	
};

typedef struct Channel {
	std::string	topic;
	std::vector<client*> Users;
	std::vector<client*>	Operators;
	bool				InviteOnly;
	bool				RestrictTopic;
	bool				SetPasswd;
	int					UserLimit;
}	t_channel;


// parsing
void parseArguments(int ac, char **av);
std::vector<Message> socketBufferParsing(client& Client, bool &closed);
Message parseCommand(std::string& raw);

#endif